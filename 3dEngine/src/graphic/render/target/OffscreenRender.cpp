#include <graphic/render/target/OffscreenRender.h>

#include <utility>
#include <graphic/helper/ImageHelper.h>
#include <graphic/setup/GraphicService.h>
#include <graphic/render/GenericRenderer.h>

namespace urchin {

    OffscreenRender::OffscreenRender(std::string name, DepthAttachmentType depthAttachmentType) :
            RenderTarget(std::move(name), depthAttachmentType),
            isInitialized(false),
            queueSubmitSemaphore(nullptr),
            queueSubmitSemaphoreUsable(false),
            commandBufferFence(nullptr) {

    }

    OffscreenRender::~OffscreenRender() {
        if (isInitialized) {
            Logger::instance().logWarning("Offscreen render not cleanup before destruction: " + getName());
            OffscreenRender::cleanup();
        }
        resetTextures();
    }

    void OffscreenRender::addTexture(const std::shared_ptr<Texture>& texture) {
        assert(!isInitialized);

        texture->enableTextureWriting(this);
        texture->initialize();
        textures.push_back(texture);
    }

    void OffscreenRender::resetTextures() {
        std::for_each(textures.begin(), textures.end(), [](const auto& t){t->removeTextureWriter();});
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
        return textures[0]->getWidth();
    }

    unsigned int OffscreenRender::getHeight() const {
        return textures[0]->getHeight();
    }

    unsigned int OffscreenRender::getLayer() const {
        return textures[0]->getLayer();
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

        for (auto& texture : textures) {
            Vector4<float> colorValue = texture->getClearColor();
            VkClearValue clearColor{};
            clearColor.color = {{colorValue[0], colorValue[1], colorValue[2], colorValue[3]}};
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
        for (const auto& texture : textures) {
            attachments.emplace_back(buildAttachment(texture->getVkFormat(), texture->hasClearColorEnabled(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
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
        for (const auto& texture : textures) {
            attachments.emplace_back(texture->getImageView());
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
        if (queueSubmitSemaphoreUsable) {
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

        if (!hasRenderer()) {
            return;
        }

        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        //fence (CPU-GPU sync) to wait completion of vkQueueSubmit
        vkWaitForFences(logicalDevice, 1, &commandBufferFence, VK_TRUE, UINT64_MAX);

        updateGraphicData(0);
        updateCommandBuffers(clearValues);

        VkSemaphore signalSemaphores[] = {queueSubmitSemaphore};
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        configureWaitSemaphore(submitInfo, nullptr);
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[0];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(logicalDevice, 1, &commandBufferFence);
        VkResult result = vkQueueSubmit(GraphicService::instance().getQueues().getGraphicsQueue(), 1, &submitInfo, commandBufferFence);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer with error code: " + std::to_string(result));
        }

        queueSubmitSemaphoreUsable = true;
    }

    void OffscreenRender::waitCommandBuffersIdle() const {
        vkWaitForFences(GraphicService::instance().getDevices().getLogicalDevice(), 1, &commandBufferFence, VK_TRUE, UINT64_MAX);
    }

}
