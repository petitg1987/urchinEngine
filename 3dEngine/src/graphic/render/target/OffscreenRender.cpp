#include <graphic/render/target/OffscreenRender.h>

#include <utility>
#include <graphic/helper/ImageHelper.h>
#include <graphic/setup/GraphicService.h>
#include <graphic/render/GenericRenderer.h>

namespace urchin {

    OffscreenRender::OffscreenRender(std::string name, DepthAttachmentType depthAttachmentType) :
            RenderTarget(std::move(name), depthAttachmentType),
            queueSubmitSemaphore(nullptr),
            queueSubmitSemaphoreUsable(false),
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
                throw std::runtime_error("Texture content should already been written on the texture to load it");
            }
        } else {
            if (loadType == LoadType::LOAD_CLEAR && !clearColor.has_value()) {
                throw std::runtime_error("Texture clear color is missing");
            }

            texture->enableTextureWriting();
            texture->initialize();
        }

        outputTextures.emplace_back(OutputTexture{texture, loadType, clearColor});
    }

    void OffscreenRender::resetOutputTextures() {
        for (auto& outputTexture : outputTextures) {
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
        vkDeviceWaitIdle(GraphicService::instance().getDevices().getLogicalDevice());

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

    std::size_t OffscreenRender::getNumFramebuffer() const {
        return 1;
    }

    void OffscreenRender::initializeClearValues() {
        if (hasDepthAttachment()) {
            VkClearValue clearDepth{};
            clearDepth.depthStencil = {1.0f, 0};
            clearValues.emplace_back(clearDepth);
        }

        for (auto& outputTexture : outputTextures) {
            Vector4<float> clearColorValue = outputTexture.clearColor.has_value() ?
                    outputTexture.clearColor.value() : Vector4<float>(1.0f, 0.5f, 0.0f, 1.0) /*orange: should never be used */;
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
            attachments.emplace_back(buildDepthAttachment(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
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
            throw std::runtime_error("Failed to create fences with error code: " + std::to_string(fenceResult));
        }

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkResult semaphoreResult = vkCreateSemaphore(GraphicService::instance().getDevices().getLogicalDevice(), &semaphoreInfo, nullptr, &queueSubmitSemaphore);
        if (semaphoreResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore with error code: " + std::to_string(semaphoreResult));
        }
    }

    void OffscreenRender::destroySyncObjects() {
        vkDestroyFence(GraphicService::instance().getDevices().getLogicalDevice(), commandBufferFence, nullptr);
        vkDestroySemaphore(GraphicService::instance().getDevices().getLogicalDevice(), queueSubmitSemaphore, nullptr);
    }

    VkSemaphore OffscreenRender::retrieveQueueSubmitSemaphoreAndFlagUsed() {
        if (queueSubmitSemaphoreUsable && hasRenderer()) {
            //Once the queue submit semaphore has been used as a wait semaphore, it cannot be re-used in the same rendering pass.
            //Examples:
            // 1) The 'deferred - first pass' pass is required for the 'ambient occlusion' pass and for the 'deferred - second pass' pass but the semaphore can be used only once. Therefore, the 'ambient occlusion' pass is the only pass which can wait for the 'deferred - first pass' pass.
            // 2) The 'shadow map' pass is required for the 'deferred - second pass' but the semaphore can be used only once. Therefore, when shadow map is cached, the wait can be done only on the first frame where the shadow map has been written in the cache.
            queueSubmitSemaphoreUsable = false;

            return queueSubmitSemaphore;
        }

        return nullptr;
    }

    void OffscreenRender::render() {
        ScopeProfiler sp(Profiler::graphic(), "offRender");

        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        //fence (CPU-GPU sync) to wait completion of vkQueueSubmit
        vkWaitForFences(logicalDevice, 1, &commandBufferFence, VK_TRUE, UINT64_MAX);

        updateTexturesWriter();
        updateGraphicData(0);
        updateCommandBuffers(0, clearValues);

        VkSemaphore signalSemaphores[] = {queueSubmitSemaphore};
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        configureWaitSemaphore(submitInfo, nullptr);
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[0];
        submitInfo.signalSemaphoreCount = hasRenderer() ? 1 : 0;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(logicalDevice, 1, &commandBufferFence);
        VkResult result = vkQueueSubmit(GraphicService::instance().getQueues().getGraphicsQueue(), 1, &submitInfo, commandBufferFence);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer with error code: " + std::to_string(result));
        }

        queueSubmitSemaphoreUsable = true;
    }

    void OffscreenRender::updateTexturesWriter() {
        for (auto& outputTexture : outputTextures) {
            outputTexture.texture->setLastTextureWriter(this);
        }

        if (depthTexture && depthTexture->isWritableTexture()) {
            if (getDepthAttachmentType() != EXTERNAL_DEPTH_ATTACHMENT) { //currently, assume that write is not done in the external depth attachment: could be not true anymore in the future !
                depthTexture->setLastTextureWriter(this);
            }
        }
    }

    bool OffscreenRender::needCommandBufferRefresh(std::size_t /*cmdBufferIndex*/) const {
        if (areRenderersDirty()) {
            return true;
        }

        return std::any_of(getRenderers().begin(), getRenderers().end(), [](const auto* renderer) {
            return renderer->isEnabled() && renderer->isDrawCommandDirty();
        });
    }

    void OffscreenRender::waitCommandBuffersIdle() const {
        //unique command buffer already idle due to 'vkWaitForFences' previously executed in 'render' method
    }

}
