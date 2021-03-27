#include "OffscreenRender.h"
#include "graphic/helper/ImageHelper.h"
#include "graphic/setup/GraphicService.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    OffscreenRender::OffscreenRender(DepthAttachmentType depthAttachmentType) :
            RenderTarget(depthAttachmentType),
            isInitialized(false),
            commandBufferFence(nullptr) {

    }

    OffscreenRender::~OffscreenRender() {
        if(isInitialized) {
            Logger::instance()->logWarning("Offscreen render not cleanup before destruction");
            OffscreenRender::cleanup();
        }
    }

    void OffscreenRender::addTexture(const std::shared_ptr<Texture>& texture) {
        assert(!isInitialized);

        texture->enableTextureWriting();
        texture->initialize();
        textures.push_back(texture);
    }

    void OffscreenRender::resetTextures() {
        textures.clear();
        if (isInitialized) {
            cleanup();
        }
    }

    void OffscreenRender::initialize() {
        assert(!isInitialized);
        assert(!textures.empty());

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
        vkDeviceWaitIdle(GraphicService::instance()->getDevices().getLogicalDevice());

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
        return textures[0]->getWidth();
    }

    unsigned int OffscreenRender::getHeight() const {
        return textures[0]->getHeight();
    }

    std::size_t OffscreenRender::getNumColorAttachment() const {
        return textures.size();
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
        VkClearValue clearColor{};
        clearColor.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        for(std::size_t i = 0; i < textures.size(); ++i) {
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
        for(const auto& texture : textures) {
            attachments.emplace_back(buildAttachment(texture->getVkFormat(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
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
        for(const auto& texture : textures) {
            attachments.emplace_back(texture->getImageView());
        }

        RenderTarget::addNewFrameBuffer(attachments);
    }

    void OffscreenRender::createSyncObjects() {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkResult fenceResult = vkCreateFence(GraphicService::instance()->getDevices().getLogicalDevice(), &fenceInfo, nullptr, &commandBufferFence);
        if (fenceResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fences with error code: " + std::to_string(fenceResult));
        }
    }

    void OffscreenRender::destroySyncObjects() {
        vkDestroyFence(GraphicService::instance()->getDevices().getLogicalDevice(), commandBufferFence, nullptr);
    }

    void OffscreenRender::render() {
        assert(!renderers.empty());
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

        updateGraphicData();
        updateCommandBuffers(clearValues);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = nullptr;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[0];
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;

        vkResetFences(logicalDevice, 1, &commandBufferFence); //ensure fence is reset just before use it
        VkResult result = vkQueueSubmit(GraphicService::instance()->getQueues().getGraphicsQueue(), 1, &submitInfo, commandBufferFence);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer with error code: " + std::to_string(result));
        }

        vkQueueWaitIdle(GraphicService::instance()->getQueues().getGraphicsQueue());
    }

    void OffscreenRender::updateGraphicData() {
        ScopeProfiler sp(Profiler::graphic(), "upGraphData");
        for (auto &renderer : renderers) {
            renderer->updateGraphicData(0);
        }
    }

    void OffscreenRender::waitCommandBuffersIdle() const {
        vkWaitForFences(GraphicService::instance()->getDevices().getLogicalDevice(), 1, &commandBufferFence, VK_TRUE, UINT64_MAX);
    }

}
