#include <libs/vkenum/vk_enum.h>
#include <utility>

#include <graphics/api/vulkan/render/target/OffscreenRender.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/api/vulkan/render/GenericRenderer.h>

namespace urchin {

    OffscreenRender::OffscreenRender(std::string name, bool isTestMode, DepthAttachmentType depthAttachmentType) :
            RenderTarget(std::move(name), isTestMode, depthAttachmentType),
            width(0),
            height(0),
            layer(0),
            commandBufferFence(nullptr),
            submitSemaphores({}),
            submitSemaphoresFrameIndex(0),
            remainingSubmitSemaphores(0),
            submitSemaphoresStale(false) {

    }

    OffscreenRender::~OffscreenRender() {
        if (isInitialized()) {
            Logger::instance().logWarning("Offscreen render not cleanup before destruction: " + getName());
            OffscreenRender::cleanup();
        }
        resetOutput();
    }

    void OffscreenRender::setOutputSize(unsigned int width, unsigned int height, unsigned int layer) {
        assert(!isInitialized());

        this->width = width;
        this->height = height;
        this->layer = layer;
    }

    void OffscreenRender::addOutputTexture(const std::shared_ptr<Texture>& texture, LoadType loadType, const std::optional<Vector4<float>>& clearColor, OutputUsage outputUsage) {
        assert(!isInitialized());
        assert(outputUsage != OutputUsage::COMPUTE || loadType == LoadType::NO_LOAD);
        assert(layer == 0 || layer == texture->getLayer());

        if (width == 0 && height == 0 && layer == 0) {
            width = texture->getWidth();
            height = texture->getHeight();
            layer = texture->getLayer();
        }

        if (loadType == LoadType::LOAD_CONTENT) {
            if (!texture->isWritableTexture()) {
                throw std::runtime_error("Content of the texture '" + texture->getName() + "' should already been written on the texture to load it for the render target: " + getName());
            }
        } else {
            if (loadType == LoadType::LOAD_CLEAR && !clearColor.has_value()) {
                throw std::runtime_error("Clear color is missing for texture '" + texture->getName() + "' on render target: " + getName());
            }

            texture->enableTextureWriting(outputUsage);
            if (!isTestMode()) {
                texture->initialize();
            }
        }

        outputTextures.emplace_back(OutputTexture{.texture=texture, .loadOperation=loadType, .clearColor=clearColor, .outputUsage=outputUsage});
    }

    std::shared_ptr<Texture>& OffscreenRender::getOutputTexture(std::size_t index) {
        assert(outputTextures.size() > index);
        return outputTextures[index].texture;
    }

    void OffscreenRender::resetOutput() {
        for (const auto& outputTexture : outputTextures) {
            if (outputTexture.texture->getLastTextureWriter() == this) {
                outputTexture.texture->setLastTextureWriter(nullptr);
            }
        }
        outputTextures.clear();
        if (hasDepthTexture() && getDepthTexture()->getLastTextureWriter() == this) {
            getDepthTexture()->setLastTextureWriter(nullptr);
        }

        if (isInitialized()) {
            cleanup();
        }

        width = 0;
        height = 0;
        layer = 0;
    }

    void OffscreenRender::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "offRenderInit");
        assert(!isInitialized());

        initializeClearValues();
        createDepthResources();
        if (!isTestMode()) {
            createRenderPass();
            createFramebuffers();
            createCommandPool();
            createCommandBuffers();
            createFence();
            createSemaphores();
        }

        initializeProcessors();
        setInitialized(true);
    }

    void OffscreenRender::cleanup() {
        assert(isInitialized());
        if (!isTestMode()) {
            VkResult result = vkDeviceWaitIdle(GraphicsSetupService::instance().getDevices().getLogicalDevice());
            if (result != VK_SUCCESS) {
                Logger::instance().logError("Failed to wait for device idle with error code '" + std::string(string_VkResult(result)) + "' on render target: " + getName());
            }
        }

        cleanupProcessors();

        if (!isTestMode()) {
            destroySemaphores();
            destroyFence();
            destroyCommandBuffersAndPool();
            destroyFramebuffers();
        }
        destroyDepthResources();
        if (!isTestMode()) {
            destroyRenderPass();
        }
        clearValues.clear();

        setInitialized(false);
    }

    unsigned int OffscreenRender::getWidth() const {
        assert(width != 0);
        return width;
    }

    unsigned int OffscreenRender::getHeight() const {
        assert(height != 0);
        return height;
    }

    unsigned int OffscreenRender::getLayer() const {
        assert(layer != 0);
        return layer;
    }

    std::size_t OffscreenRender::getNumColorAttachment() const {
        return outputTextures.size();
    }

    std::size_t OffscreenRender::getNumFramebuffer() const {
        return 1;
    }

    void OffscreenRender::initializeClearValues() {
        bool hasLoadClear = hasDepthAttachment() && getDepthAttachmentType() != EXTERNAL_DEPTH_ATTACHMENT;
        for (const auto& outputTexture : outputTextures) {
            hasLoadClear = hasLoadClear || outputTexture.loadOperation == LoadType::LOAD_CLEAR;
        }

        if (hasLoadClear) {
            if (hasDepthAttachment()) {
                VkClearValue clearDepth{};
                clearDepth.depthStencil = {.depth=1.0f, .stencil=0};
                clearValues.emplace_back(clearDepth);
            }

            for (const auto& outputTexture: outputTextures) {
                Vector4<float> clearColorValue = outputTexture.clearColor.value_or(Vector4<float>(1.0f, 0.5f, 0.0f, 1.0) /*orange: should never be used */);
                VkClearValue clearColor{};
                clearColor.color = {{clearColorValue[0], clearColorValue[1], clearColorValue[2], clearColorValue[3]}};
                clearValues.emplace_back(clearColor);
            }
        }
    }

    void OffscreenRender::createRenderPass() {
        if (couldHaveGraphicsProcessors()) {
            std::vector<VkAttachmentDescription2> attachments;
            uint32_t attachmentIndex = 0;

            VkAttachmentReference2 depthAttachmentRef{};
            depthAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
            if (hasDepthAttachment()) {
                attachments.emplace_back(buildDepthAttachment(getDepthTexture()->getVkFormat(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL));
                depthAttachmentRef.attachment = attachmentIndex++;
                depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }

            std::vector<VkAttachmentReference2> colorAttachmentRefs;
            for (const auto& outputTexture : outputTextures) {
                bool clearOnLoad = outputTexture.clearColor.has_value();
                bool loadContent = outputTexture.loadOperation == LoadType::LOAD_CONTENT;
                VkImageLayout finalLayout = outputTexture.texture->getOutputUsage() == OutputUsage::GRAPHICS ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_GENERAL;
                attachments.emplace_back(buildAttachment(outputTexture.texture->getVkFormat(), clearOnLoad, loadContent, finalLayout));
                VkAttachmentReference2 colorAttachmentRef{};
                colorAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                colorAttachmentRef.attachment = attachmentIndex++;
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachmentRefs.push_back(colorAttachmentRef);
            }

            RenderTarget::createRenderPass(depthAttachmentRef, colorAttachmentRefs, attachments);
        }
    }

    void OffscreenRender::createFramebuffers() {
        if (couldHaveGraphicsProcessors()) {
            std::vector<std::vector<VkImageView>> attachments;
            attachments.resize(getLayer());

            for (std::size_t layerIndex = 0; layerIndex < getLayer(); ++layerIndex) {
                attachments[layerIndex].reserve((hasDepthAttachment() ? 1 : 0) + outputTextures.size());
                if (hasDepthAttachment()) {
                    std::vector<VkImageView> depthImageView = getDepthTexture()->getWritableImageViews();
                    attachments[layerIndex].emplace_back(depthImageView.at(layerIndex));
                }
                for (const auto& outputTexture: outputTextures) {
                    std::vector<VkImageView> outputTextureImageViews = outputTexture.texture->getWritableImageViews();
                    attachments[layerIndex].emplace_back(outputTextureImageViews.at(layerIndex));
                }
            }

            addFramebuffers(attachments);
        }
    }

    void OffscreenRender::createFence() {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkResult fenceResult = vkCreateFence(GraphicsSetupService::instance().getDevices().getLogicalDevice(), &fenceInfo, nullptr, &commandBufferFence);
        if (fenceResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fences with error code '" + std::string(string_VkResult(fenceResult)) + "' on render target: " + getName());
        }
    }

    void OffscreenRender::destroyFence() const {
        vkDestroyFence(GraphicsSetupService::instance().getDevices().getLogicalDevice(), commandBufferFence, nullptr);
    }

    void OffscreenRender::createSemaphores() {
        for (VkSemaphore& submitSemaphore : submitSemaphores) {
            VkSemaphoreCreateInfo semaphoreCreateInfo{};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VkResult semaphoreResult = vkCreateSemaphore(GraphicsSetupService::instance().getDevices().getLogicalDevice(), &semaphoreCreateInfo, nullptr, &submitSemaphore);
            if (semaphoreResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create semaphore with error code '" + std::string(string_VkResult(semaphoreResult)) + "' on render target: " + getName());
            }
        }
    }

    void OffscreenRender::destroySemaphores() const {
        for (const VkSemaphore& submitSemaphore : submitSemaphores) {
            vkDestroySemaphore(GraphicsSetupService::instance().getDevices().getLogicalDevice(), submitSemaphore, nullptr);
        }
    }

    void OffscreenRender::render(uint32_t frameIndex, unsigned int numDependenciesToOutputs) {
        ScopeProfiler sp(Profiler::graphic(), "offRender");
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        //fence (CPU-GPU sync) to wait completion of vkQueueSubmit2
        VkResult resultWaitForFences = vkWaitForFences(logicalDevice, 1, &commandBufferFence, VK_TRUE, UINT64_MAX);
        if (resultWaitForFences != VK_SUCCESS && resultWaitForFences != VK_TIMEOUT) {
            throw std::runtime_error("Failed to wait for fence with error code '" + std::string(string_VkResult(resultWaitForFences)) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }

        if (numDependenciesToOutputs > MAX_SUBMIT_SEMAPHORES) {
            throw std::runtime_error("Number of dependencies to output (" + std::to_string(numDependenciesToOutputs) + ") is higher that the maximum expected on render target: " + getName() + "/" + std::to_string(frameIndex));
        } else if (remainingSubmitSemaphores != 0) {
            throw std::runtime_error("Not all submit semaphores (remaining: " + std::to_string(remainingSubmitSemaphores) + ") has been consumed on render target: " + getName() + "/" + std::to_string(frameIndex));
        } else if (submitSemaphoresStale) {
            VkResult resultDeviceWait = vkDeviceWaitIdle(GraphicsSetupService::instance().getDevices().getLogicalDevice());
            if (resultDeviceWait != VK_SUCCESS) {
                Logger::instance().logError("Failed to wait for device idle with error code '" + std::string(string_VkResult(resultDeviceWait)) + "' on render target: " + getName());
            }
            destroySemaphores();
            createSemaphores();
            submitSemaphoresStale = false;
        }

        updatePipelineProcessorData(0);
        updateCommandBuffers(0, clearValues);

        VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
        commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        commandBufferSubmitInfo.commandBuffer = getCommandBuffer(0);
        commandBufferSubmitInfo.deviceMask = 0;

        std::array<VkSemaphoreSubmitInfo, MAX_SUBMIT_SEMAPHORES> submitSemaphoreSubmitInfo{};
        for (unsigned int i = 0; i < numDependenciesToOutputs; ++i) {
            VkSemaphoreSubmitInfo semaphoreSubmitInfo{};
            semaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
            semaphoreSubmitInfo.semaphore = submitSemaphores[i];
            semaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
            submitSemaphoreSubmitInfo[i] = semaphoreSubmitInfo;
        }

        VkSubmitInfo2 submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        configureWaitSemaphore(frameIndex, submitInfo, std::nullopt);
        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &commandBufferSubmitInfo;
        submitInfo.signalSemaphoreInfoCount = numDependenciesToOutputs;
        submitInfo.pSignalSemaphoreInfos = numDependenciesToOutputs > 0 ? submitSemaphoreSubmitInfo.data() : nullptr;
        remainingSubmitSemaphores = numDependenciesToOutputs;
        submitSemaphoresFrameIndex = frameIndex;

        updateTexturesWriter();

        VkResult resultResetFences = vkResetFences(logicalDevice, 1, &commandBufferFence);
        if (resultResetFences != VK_SUCCESS) {
            throw std::runtime_error("Failed to reset fences with error code '" + std::string(string_VkResult(resultResetFences)) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }
        VkResult resultQueueSubmit = vkQueueSubmit2(GraphicsSetupService::instance().getQueues().getGraphicsAndComputeQueue(), 1, &submitInfo, commandBufferFence);
        if (resultQueueSubmit != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit queue with error code '" + std::string(string_VkResult(resultQueueSubmit)) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }
    }

    void OffscreenRender::updateTexturesWriter() {
        for (const auto& outputTexture : outputTextures) {
            outputTexture.texture->setLastTextureWriter(this);
        }

        if (hasDepthTexture() && getDepthTexture()->isWritableTexture()
                && getDepthAttachmentType() != EXTERNAL_DEPTH_ATTACHMENT /* currently, assume that write is not done in the external depth attachment: could be not true anymore in the future ! */) {
            getDepthTexture()->setLastTextureWriter(this);
        }
    }

    void OffscreenRender::fillAdditionalOffscreenRenderDependencies(std::vector<OffscreenRender*> &offscreenRenderDependencies) const {
        for (const auto& outputTexture : outputTextures) {
            if (outputTexture.loadOperation == LoadType::LOAD_CONTENT) {
                offscreenRenderDependencies.push_back(outputTexture.texture->getLastTextureWriter());
            }
        }
    }

    VkSemaphore OffscreenRender::popSubmitSemaphore(uint32_t frameIndex, const std::string& requester) {
        if (submitSemaphoresFrameIndex == frameIndex) {
            if (remainingSubmitSemaphores == 0) {
                throw std::runtime_error("No more submit semaphore available on render target: " + getName() + "/" + std::to_string(frameIndex) + " (requester: " + requester + ")");
            }
            return submitSemaphores[--remainingSubmitSemaphores];
        }

        //This render target has been generated in a previous frame: therefore, the synchronization is already done and no need to be redone. Typical case is when the render target is cached.
        assert(submitSemaphoresFrameIndex < frameIndex);
        return nullptr;
    }

    void OffscreenRender::markSubmitSemaphoreUnused(uint32_t frameIndex) {
        popSubmitSemaphore(frameIndex, getName());
        submitSemaphoresStale = true; //an unused semaphore is considered as stale
    }

    bool OffscreenRender::needCommandBufferRefresh(std::size_t frameIndex) const {
        if (areProcessorsOrCopiersDirty()) {
            return true;
        }

        return std::ranges::any_of(getProcessors(), [frameIndex](const auto* renderer) {
            return renderer->isEnabled() && renderer->needCommandBufferRefresh(frameIndex);
        });
    }

    void OffscreenRender::waitCommandBuffersIdle() const {
        //unique command buffer already idle due to 'vkWaitForFences' previously executed in 'render' method
    }

}
