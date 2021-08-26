#include <stdexcept>
#include <utility>
#include <thread>

#include <graphic/render/target/ScreenRender.h>
#include <graphic/setup/GraphicService.h>
#include <graphic/helper/ImageHelper.h>
#include <graphic/render/GenericRenderer.h>
#include <graphic/capture/CaptureService.h>

namespace urchin {

    //static
    const std::size_t ScreenRender::MAX_CONCURRENT_FRAMES = 2;

    ScreenRender::ScreenRender(std::string name, DepthAttachmentType depthAttachmentType) :
            RenderTarget(std::move(name), depthAttachmentType),
            verticalSyncEnabled(true),
            vkImageIndex(std::numeric_limits<uint32_t>::max()) {

    }

    ScreenRender::~ScreenRender() {
        if (isInitialized) {
            Logger::instance().logWarning("Screen render not cleanup before destruction");
            ScreenRender::cleanup();
        }
    }

    void ScreenRender::initialize() {
        assert(!isInitialized);

        initializeClearValues();
        swapChainHandler.initialize(verticalSyncEnabled);
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createCommandPool();
        createCommandBuffers();
        createSyncObjects();

        initializeRenderers();

        isInitialized = true;
    }

    void ScreenRender::cleanup() {
        if (isInitialized) {
            vkDeviceWaitIdle(GraphicService::instance().getDevices().getLogicalDevice());

            cleanupRenderers();

            destroySyncObjects();
            destroyCommandBuffersAndPool();
            destroyFramebuffers();
            destroyDepthResources();
            destroyRenderPass();
            destroyImageViews();
            swapChainHandler.cleanup();
            clearValues.clear();

            isInitialized = false;
        }
    }

    void ScreenRender::onResize() {
        unsigned int sceneWidth, sceneHeight;
        GraphicService::instance().getFramebufferSizeRetriever()->getFramebufferSizeInPixel(sceneWidth, sceneHeight);

        if (sceneWidth > 1 && sceneHeight > 1) { //size is generally invalid when window is minimized on Windows
            cleanup();
            initialize();
        }
    }

    void ScreenRender::updateVerticalSync(bool verticalSyncEnabled) {
        if (this->verticalSyncEnabled != verticalSyncEnabled) {
            this->verticalSyncEnabled = verticalSyncEnabled;

            cleanup();
            initialize();
        }
    }

    unsigned int ScreenRender::getWidth() const {
        return swapChainHandler.getSwapChainExtent().width;
    }

    unsigned int ScreenRender::getHeight() const {
        return swapChainHandler.getSwapChainExtent().height;
    }

    unsigned int ScreenRender::getLayer() const {
        return 1;
    }

    std::size_t ScreenRender::getNumFramebuffer() const {
        return swapChainHandler.getSwapChainImages().size();
    }

    std::size_t ScreenRender::getNumColorAttachment() const {
        return 1;
    }

    void ScreenRender::takeScreenshot(const std::string& filename, unsigned int dstWidth, unsigned int dstHeight) const {
        assert(vkImageIndex != std::numeric_limits<uint32_t>::max());
        VkImage srcImage = swapChainHandler.getSwapChainImages()[vkImageIndex];
        CaptureService::instance().takeCapture(filename, srcImage, swapChainHandler.getImageFormat(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, getWidth(), getHeight(), dstWidth, dstHeight);
    }

    void ScreenRender::initializeClearValues() {
        if (hasDepthAttachment()) {
            VkClearValue clearDepth{};
            clearDepth.depthStencil = {1.0f, 0};
            clearValues.emplace_back(clearDepth);
        }

        VkClearValue clearColor{};
        clearColor.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues.emplace_back(clearColor);
    }

    void ScreenRender::createImageViews() {
        swapChainImageViews.resize(swapChainHandler.getSwapChainImages().size());

        for (size_t i = 0; i < swapChainHandler.getSwapChainImages().size(); i++) {
            VkImage image = swapChainHandler.getSwapChainImages()[i];
            swapChainImageViews[i] = ImageHelper::createImageView(image, VK_IMAGE_VIEW_TYPE_2D, swapChainHandler.getImageFormat(), VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
        }
    }

    void ScreenRender::destroyImageViews() {
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(GraphicService::instance().getDevices().getLogicalDevice(), imageView, nullptr);
        }
    }

    void ScreenRender::createRenderPass() {
        std::vector<VkAttachmentDescription> attachments;
        uint32_t attachmentIndex = 0;

        VkAttachmentReference depthAttachmentRef{};
        if (hasDepthAttachment()) {
            attachments.emplace_back(buildDepthAttachment(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
            depthAttachmentRef.attachment = attachmentIndex++;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        std::vector<VkAttachmentReference> colorAttachmentRefs;
        attachments.emplace_back(buildAttachment(swapChainHandler.getImageFormat(), false, false, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = attachmentIndex;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentRefs.push_back(colorAttachmentRef);

        RenderTarget::createRenderPass(depthAttachmentRef, colorAttachmentRefs, attachments);
    }

    void ScreenRender::createFramebuffers() {
        for (auto& swapChainImageView : swapChainImageViews) {
            std::vector<VkImageView> attachments;
            if (hasDepthAttachment()) {
                attachments.emplace_back(depthTexture->getImageView());
            }
            attachments.emplace_back(swapChainImageView);

            RenderTarget::addNewFrameBuffer(attachments);
        }
    }

    void ScreenRender::createSyncObjects() {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        imageAvailableSemaphores.resize(MAX_CONCURRENT_FRAMES);
        renderFinishedSemaphores.resize(MAX_CONCURRENT_FRAMES);
        commandBufferFences.resize(MAX_CONCURRENT_FRAMES);
        imagesFences.resize(swapChainImageViews.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (std::size_t i = 0; i < MAX_CONCURRENT_FRAMES; i++) {
            VkResult imageAvailableSemaphoreResult = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
            if (imageAvailableSemaphoreResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create image available semaphore with error code: " + std::to_string(imageAvailableSemaphoreResult));
            }

            VkResult renderFinishedSemaphoreResult = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
            if (renderFinishedSemaphoreResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create render finished semaphore with error code: " + std::to_string(renderFinishedSemaphoreResult));
            }

            VkResult fenceResult = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &commandBufferFences[i]);
            if (fenceResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create fences with error code: " + std::to_string(fenceResult));
            }
        }
    }

    void ScreenRender::destroySyncObjects() {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        imagesFences.clear();
        for (std::size_t i = 0; i < MAX_CONCURRENT_FRAMES; i++) {
            vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(logicalDevice, commandBufferFences[i], nullptr);
        }
    }

    void ScreenRender::render() {
        ScopeProfiler sp(Profiler::graphic(), "screenRender");

        if (!hasRenderer()) {
            return;
        }

        static size_t currentFrameIndex = 0;
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        //fence (CPU-GPU sync) to wait completion of vkQueueSubmit for the frame 'currentFrameIndex'
        vkWaitForFences(logicalDevice, 1, &commandBufferFences[currentFrameIndex], VK_TRUE, UINT64_MAX);

        VkResult resultAcquireImage = vkAcquireNextImageKHR(logicalDevice, swapChainHandler.getSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrameIndex], VK_NULL_HANDLE, &vkImageIndex);
        if (resultAcquireImage == VK_ERROR_OUT_OF_DATE_KHR) {
            onResize();
            return;
        } else if (resultAcquireImage != VK_SUCCESS && resultAcquireImage != VK_SUBOPTIMAL_KHR /* Continue with sub optimal image because already acquired */) {
            throw std::runtime_error("Failed to acquire swap chain image with error code: " + std::to_string(resultAcquireImage));
        }

        updateGraphicData(vkImageIndex);
        updateCommandBuffers(clearValues);

        if (imagesFences[vkImageIndex] != VK_NULL_HANDLE) {
            //Fence (CPU-GPU sync) to wait if a previous frame is using this image. Useful in 2 cases:
            // 1) MAX_CONCURRENT_FRAMES > swapChainHandler.getSwapChainImages().size()
            // 2) Acquired images from swap chain are returned out-of-order
            vkWaitForFences(logicalDevice, 1, &imagesFences[vkImageIndex], VK_TRUE, UINT64_MAX);
        }
        imagesFences[vkImageIndex] = commandBufferFences[currentFrameIndex]; //mark the image as now being in use by this frame

        VkSemaphore queuePresentWaitSemaphores[] = {renderFinishedSemaphores[currentFrameIndex] /* semaphores (GPU-GPU sync) to wait command buffers execution before present the image */};
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        configureWaitSemaphore(submitInfo, imageAvailableSemaphores[currentFrameIndex] /* semaphores (GPU-GPU sync) to wait image available before executing command buffers */);
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[vkImageIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = queuePresentWaitSemaphores;

        vkResetFences(logicalDevice, 1, &commandBufferFences[currentFrameIndex]);
        VkResult result = vkQueueSubmit(GraphicService::instance().getQueues().getGraphicsQueue(), 1, &submitInfo, commandBufferFences[currentFrameIndex]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer with error code: " + std::to_string(result));
        }

        VkSwapchainKHR swapChains[] = {swapChainHandler.getSwapChain()};
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = queuePresentWaitSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &vkImageIndex;
        presentInfo.pResults = nullptr;

        VkResult queuePresentResult = vkQueuePresentKHR(GraphicService::instance().getQueues().getPresentationQueue(), &presentInfo);
        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR) {
            onResize();
        } else if (queuePresentResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to acquire swap chain image with error code: " + std::to_string(resultAcquireImage));
        }

        currentFrameIndex = (currentFrameIndex + 1) % MAX_CONCURRENT_FRAMES;
    }

    void ScreenRender::waitCommandBuffersIdle() const {
        for (unsigned int i = 0; i < MAX_CONCURRENT_FRAMES; ++i) {
            vkWaitForFences(GraphicService::instance().getDevices().getLogicalDevice(), 1, &commandBufferFences[i], VK_TRUE, UINT64_MAX);
        }
    }

}
