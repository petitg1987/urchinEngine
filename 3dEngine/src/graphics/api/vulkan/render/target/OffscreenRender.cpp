#include <vulkan/vk_enum_string_helper.h>
#include <utility>

#include <graphics/api/vulkan/render/target/OffscreenRender.h>
#include <graphics/api/vulkan/helper/ImageHelper.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/api/vulkan/render/GenericRenderer.h>

namespace urchin {

    OffscreenRender::OffscreenRender(std::string name, DepthAttachmentType depthAttachmentType) :
            RenderTarget(std::move(name), depthAttachmentType),
            commandBufferFence(nullptr),
            submitSemaphores({}),
            submitSemaphoresFrameIndex(0),
            remainingSubmitSemaphores(0),
            submitSemaphoresStale(false) {

    }

    OffscreenRender::~OffscreenRender() {
        if (isInitialized) {
            Logger::instance().logWarning("Offscreen render not cleanup before destruction: " + getName());
            OffscreenRender::cleanup();
        }
        resetOutputTextures();
    }

    void OffscreenRender::addOutputTexture(const std::shared_ptr<Texture>& texture, LoadType loadType, std::optional<Vector4<float>> clearColor, OutputUsage outputUsage) {
        assert(!isInitialized);
        assert(outputUsage != OutputUsage::COMPUTE || loadType == LoadType::NO_LOAD);

        if (loadType == LoadType::LOAD_CONTENT) {
            if (!texture->isWritableTexture()) {
                throw std::runtime_error("Content of the texture '" + texture->getName() + "' should already been written on the texture to load it for the render target: " + getName());
            }
        } else {
            if (loadType == LoadType::LOAD_CLEAR && !clearColor.has_value()) {
                throw std::runtime_error("Clear color is missing for texture '" + texture->getName() + "' on render target: " + getName());
            }

            texture->enableTextureWriting(outputUsage);
            texture->initialize();
        }

        outputTextures.emplace_back(OutputTexture{texture, loadType, clearColor, outputUsage});
    }

    void OffscreenRender::resetOutputTextures() {
        for (const auto& outputTexture : outputTextures) {
            if (outputTexture.texture->getLastTextureWriter() == this) {
                outputTexture.texture->setLastTextureWriter(nullptr);
            }
        }
        outputTextures.clear();
        if (depthTexture && depthTexture->getLastTextureWriter() == this) {
            depthTexture->setLastTextureWriter(nullptr);
        }

        if (isInitialized) {
            cleanup();
        }
    }

    std::shared_ptr<Texture>& OffscreenRender::getOutputTexture(std::size_t index) {
        assert(outputTextures.size() > index);
        return outputTextures[index].texture;
    }

    void OffscreenRender::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "offRenderInit");
        assert(!isInitialized);
        assert(!outputTextures.empty());

        initializeClearValues();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createCommandPool();
        createCommandBuffers();
        createFence();
        createSemaphores();

        initializeProcessors();

        isInitialized = true;
    }

    void OffscreenRender::cleanup() {
        assert(isInitialized);
        VkResult result = vkDeviceWaitIdle(GraphicsSetupService::instance().getDevices().getLogicalDevice());
        if (result != VK_SUCCESS) {
            Logger::instance().logError("Failed to wait for device idle with error code '" + std::string(string_VkResult(result)) + "' on render target: " + getName());
        }

        cleanupProcessors();

        destroySemaphores();
        destroyFence();
        destroyCommandBuffersAndPool();
        destroyFramebuffers();
        destroyDepthResources();
        destroyRenderPass();
        clearValues.clear();

        isInitialized = false;
    }

    unsigned int OffscreenRender::getWidth() const {
        assert(!outputTextures.empty());
        return outputTextures[0].texture->getWidth();
    }

    unsigned int OffscreenRender::getHeight() const {
        assert(!outputTextures.empty());
        return outputTextures[0].texture->getHeight();
    }

    unsigned int OffscreenRender::getLayer() const {
        assert(!outputTextures.empty());
        return outputTextures[0].texture->getLayer();
    }

    std::size_t OffscreenRender::getNumColorAttachment() const {
        return outputTextures.size();
    }

    std::size_t OffscreenRender::getNumFramebuffer() const {
        return 1;
    }

    void OffscreenRender::initializeClearValues() {
        bool hasLoadClear = hasDepthAttachment();
        for (const auto& outputTexture : outputTextures) {
            hasLoadClear = hasLoadClear || outputTexture.loadOperation == LoadType::LOAD_CLEAR;
        }

        if (hasLoadClear) {
            if (hasDepthAttachment()) {
                VkClearValue clearDepth{};
                clearDepth.depthStencil = {1.0f, 0};
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
            std::vector<VkAttachmentDescription> attachments;
            uint32_t attachmentIndex = 0;

            VkAttachmentReference depthAttachmentRef{};
            if (hasDepthAttachment()) {
                attachments.emplace_back(buildDepthAttachment(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL));
                depthAttachmentRef.attachment = attachmentIndex++;
                depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }

            std::vector<VkAttachmentReference> colorAttachmentRefs;
            for (const auto& outputTexture: outputTextures) {
                bool clearOnLoad = outputTexture.clearColor.has_value();
                bool loadContent = outputTexture.loadOperation == LoadType::LOAD_CONTENT;
                attachments.emplace_back(buildAttachment(outputTexture.texture->getVkFormat(), clearOnLoad, loadContent, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
                VkAttachmentReference colorAttachmentRef{};
                colorAttachmentRef.attachment = attachmentIndex++;
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachmentRefs.push_back(colorAttachmentRef);
            }

            RenderTarget::createRenderPass(depthAttachmentRef, colorAttachmentRefs, attachments);
        }
    }

    void OffscreenRender::createFramebuffers() {
        if (couldHaveGraphicsProcessors()) {
            std::vector<VkImageView> attachments;
            if (hasDepthAttachment()) {
                attachments.emplace_back(depthTexture->getImageView());
            }
            for (const auto& outputTexture: outputTextures) {
                attachments.emplace_back(outputTexture.texture->getImageView());
            }

            RenderTarget::addNewFrameBuffer(attachments);
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

    void OffscreenRender::destroyFence() {
        vkDestroyFence(GraphicsSetupService::instance().getDevices().getLogicalDevice(), commandBufferFence, nullptr);
    }

    void OffscreenRender::createSemaphores() {
        for (VkSemaphore& submitSemaphore : submitSemaphores) {
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VkResult semaphoreResult = vkCreateSemaphore(GraphicsSetupService::instance().getDevices().getLogicalDevice(), &semaphoreInfo, nullptr, &submitSemaphore);
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

    void OffscreenRender::render(std::uint32_t frameIndex, unsigned int numDependenciesToOutputs) {
        ScopeProfiler sp(Profiler::graphic(), "offRender");
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        //fence (CPU-GPU sync) to wait completion of vkQueueSubmit
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

        updateTexturesWriter();
        updatePipelineProcessorData(0);
        updateCommandBuffers(0, clearValues);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        configureWaitSemaphore(frameIndex, submitInfo, std::nullopt);
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffers.data();
        submitInfo.signalSemaphoreCount = numDependenciesToOutputs;
        submitInfo.pSignalSemaphores = submitSemaphores.data();
        remainingSubmitSemaphores = numDependenciesToOutputs;
        submitSemaphoresFrameIndex = frameIndex;

        VkResult resultResetFences = vkResetFences(logicalDevice, 1, &commandBufferFence);
        if (resultResetFences != VK_SUCCESS) {
            throw std::runtime_error("Failed to reset fences with error code '" + std::string(string_VkResult(resultResetFences)) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }
        VkResult resultQueueSubmit = vkQueueSubmit(GraphicsSetupService::instance().getQueues().getGraphicsAndComputeQueue(), 1, &submitInfo, commandBufferFence);
        if (resultQueueSubmit != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit queue with error code '" + std::string(string_VkResult(resultQueueSubmit)) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }
    }

    void OffscreenRender::updateTexturesWriter() {
        for (const auto& outputTexture : outputTextures) {
            outputTexture.texture->setLastTextureWriter(this);
        }

        if (depthTexture && depthTexture->isWritableTexture()
                && getDepthAttachmentType() != EXTERNAL_DEPTH_ATTACHMENT /* currently, assume that write is not done in the external depth attachment: could be not true anymore in the future ! */) {
            depthTexture->setLastTextureWriter(this);
        }
    }

    VkSemaphore OffscreenRender::popSubmitSemaphore(std::uint32_t frameIndex) {
        if (submitSemaphoresFrameIndex == frameIndex) {
            if (remainingSubmitSemaphores == 0) {
                throw std::runtime_error("No more submit semaphore available on render target: " + getName() + "/" + std::to_string(frameIndex));
            }
            return submitSemaphores[--remainingSubmitSemaphores];
        }

        //This render target has been generated in a previous frame: therefore, the synchronization is already done and no need to be redone. Typical case is when the render target is cached.
        assert(submitSemaphoresFrameIndex < frameIndex);
        return nullptr;
    }

    void OffscreenRender::markSubmitSemaphoreUnused(std::uint32_t frameIndex) {
        popSubmitSemaphore(frameIndex);
        submitSemaphoresStale = true; //an unused semaphore is considered as stale
    }

    bool OffscreenRender::needCommandBufferRefresh(std::size_t frameIndex) const {
        if (areProcessorsDirty()) {
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
