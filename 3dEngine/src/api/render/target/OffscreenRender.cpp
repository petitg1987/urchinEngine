#include <api/render/target/OffscreenRender.h>

#include <utility>
#include <api/helper/ImageHelper.h>
#include <api/setup/GraphicService.h>
#include <api/render/GenericRenderer.h>

namespace urchin {

    OffscreenRender::OffscreenRender(std::string name, DepthAttachmentType depthAttachmentType) :
            RenderTarget(std::move(name), depthAttachmentType),
            queueSubmitSemaphores({}),
            renderTargetUsageCount(0),
            commandBufferFence(nullptr) {

    }

    OffscreenRender::~OffscreenRender() {
        if (isInitialized) {
            Logger::instance().logWarning("Offscreen render not cleanup before destruction: " + getName());
            OffscreenRender::cleanup();
        }
        resetOutputTextures();
    }

    void OffscreenRender::addOutputTexture(const std::shared_ptr<Texture>& texture, LoadType loadType, std::optional<Vector4<float>> clearColor) {
        assert(!isInitialized);

        if (loadType == LoadType::LOAD_CONTENT) {
            if (!texture->isWritableTexture()) {
                throw std::runtime_error("Content of the texture '" + texture->getName() + "' should already been written on the texture to load it for the render target: " + getName());
            }
        } else {
            if (loadType == LoadType::LOAD_CLEAR && !clearColor.has_value()) {
                throw std::runtime_error("Clear color is missing for texture '" + texture->getName() + "' on render target: " + getName());
            }

            texture->enableTextureWriting();
            texture->initialize();
        }

        outputTextures.emplace_back(OutputTexture{texture, loadType, clearColor});
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
        createSyncObjects();

        initializeRenderers();

        isInitialized = true;
    }

    void OffscreenRender::cleanup() {
        assert(isInitialized);
        VkResult result = vkDeviceWaitIdle(GraphicService::instance().getDevices().getLogicalDevice());
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to wait for device idle with error code '" + std::to_string(result) + "' on render target: " + getName());
        }

        cleanupRenderers();

        destroySyncObjects();
        destroyCommandBuffersAndPool();
        destroyFramebuffers();
        destroyDepthResources();
        destroyRenderPass();
        clearValues.clear();

        isInitialized = false;
    }

    unsigned int OffscreenRender::getWidth() const {
        return outputTextures[0].texture->getWidth();
    }

    unsigned int OffscreenRender::getHeight() const {
        return outputTextures[0].texture->getHeight();
    }

    unsigned int OffscreenRender::getLayer() const {
        return outputTextures[0].texture->getLayer();
    }

    std::size_t OffscreenRender::getNumColorAttachment() const {
        return outputTextures.size();
    }

    std::size_t OffscreenRender::hasOutputTextureWithContentToLoad() const {
        for (const OutputTexture& outputTexture : outputTextures) {
            if (outputTexture.loadOperation == LoadType::LOAD_CONTENT) {
                return true;
            }
        }
        return false;
    }

    std::size_t OffscreenRender::getNumFramebuffer() const {
        return 1;
    }

    void OffscreenRender::initializeClearValues() {
        if (hasDepthAttachment()) {
            VkClearValue clearDepth{};
            clearDepth.depthStencil = {1.0f, 0};
            clearValues.emplace_back(clearDepth);
        }

        for (const auto& outputTexture : outputTextures) {
            Vector4<float> clearColorValue = outputTexture.clearColor.value_or(Vector4<float>(1.0f, 0.5f, 0.0f, 1.0) /*orange: should never be used */);
            VkClearValue clearColor{};
            clearColor.color = {{clearColorValue[0], clearColorValue[1], clearColorValue[2], clearColorValue[3]}};
            clearValues.emplace_back(clearColor);
        }
    }

    void OffscreenRender::createRenderPass() {
        std::vector<VkAttachmentDescription> attachments;
        uint32_t attachmentIndex = 0;

        VkAttachmentReference depthAttachmentRef{};
        if (hasDepthAttachment()) {
            attachments.emplace_back(buildDepthAttachment(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL));
            depthAttachmentRef.attachment = attachmentIndex++;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        std::vector<VkAttachmentReference> colorAttachmentRefs;
        for (const auto& outputTexture : outputTextures) {
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

    void OffscreenRender::createFramebuffers() {
        std::vector<VkImageView> attachments;
        if (hasDepthAttachment()) {
            attachments.emplace_back(depthTexture->getImageView());
        }
        for (const auto& outputTexture : outputTextures) {
            attachments.emplace_back(outputTexture.texture->getImageView());
        }

        RenderTarget::addNewFrameBuffer(attachments);
    }

    void OffscreenRender::createSyncObjects() {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkResult fenceResult = vkCreateFence(GraphicService::instance().getDevices().getLogicalDevice(), &fenceInfo, nullptr, &commandBufferFence);
        if (fenceResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fences with error code '" + std::to_string(fenceResult) + "' on render target: " + getName());
        }

        for (VkSemaphore& queueSubmitSemaphore : queueSubmitSemaphores) {
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VkResult semaphoreResult = vkCreateSemaphore(GraphicService::instance().getDevices().getLogicalDevice(), &semaphoreInfo, nullptr, &queueSubmitSemaphore);
            if (semaphoreResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create semaphore with error code '" + std::to_string(semaphoreResult) + "' on render target: " + getName());
            }
        }
    }

    void OffscreenRender::destroySyncObjects() {
        vkDestroyFence(GraphicService::instance().getDevices().getLogicalDevice(), commandBufferFence, nullptr);
        for (VkSemaphore& queueSubmitSemaphore : queueSubmitSemaphores) {
            vkDestroySemaphore(GraphicService::instance().getDevices().getLogicalDevice(), queueSubmitSemaphore, nullptr);
        }
    }

    VkSemaphore OffscreenRender::popQueueSubmitSemaphore() {
        assert(renderTargetUsageCount >= 1); //TODO use exception !
        --renderTargetUsageCount;
        return queueSubmitSemaphores[renderTargetUsageCount];
    }

    void OffscreenRender::render(unsigned int renderTargetUsageCount) { //TODO review name
        ScopeProfiler sp(Profiler::graphic(), "offRender");

        assert(renderTargetUsageCount <= queueSubmitSemaphores.size()); //TODO use exception !
        assert(this->renderTargetUsageCount == 0); //TODO use exception !
        this->renderTargetUsageCount = renderTargetUsageCount;

        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        //fence (CPU-GPU sync) to wait completion of vkQueueSubmit
        VkResult resultWaitForFences = vkWaitForFences(logicalDevice, 1, &commandBufferFence, VK_TRUE, UINT64_MAX);
        if (resultWaitForFences != VK_SUCCESS && resultWaitForFences != VK_TIMEOUT) {
            throw std::runtime_error("Failed to wait for fence with error code '" + std::to_string(resultWaitForFences) + "' on render target: " + getName());
        }

        updateTexturesWriter();
        updateGraphicData(0);
        updateCommandBuffers(0, clearValues);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        configureWaitSemaphore(submitInfo, std::nullopt);
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[0];
        submitInfo.signalSemaphoreCount = renderTargetUsageCount;
        submitInfo.pSignalSemaphores = queueSubmitSemaphores.data();

        VkResult resultResetFences = vkResetFences(logicalDevice, 1, &commandBufferFence);
        if (resultResetFences != VK_SUCCESS) {
            throw std::runtime_error("Failed to reset fences with error code '" + std::to_string(resultResetFences) + "' on render target: " + getName());
        }
        VkResult resultQueueSubmit = vkQueueSubmit(GraphicService::instance().getQueues().getGraphicsQueue(), 1, &submitInfo, commandBufferFence);
        if (resultQueueSubmit != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit queue with error code '" + std::to_string(resultQueueSubmit) + "' on render target: " + getName());
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

    bool OffscreenRender::needCommandBufferRefresh(std::size_t /*cmdBufferIndex*/) const {
        if (areRenderersDirty()) {
            return true;
        }

        return std::ranges::any_of(getRenderers(), [](const auto* renderer) {
            return renderer->isEnabled() && renderer->isDrawCommandDirty();
        });
    }

    void OffscreenRender::waitCommandBuffersIdle() const {
        //unique command buffer already idle due to 'vkWaitForFences' previously executed in 'render' method
    }

}
