#include <stdexcept>
#include <utility>
#include <thread>

#include <api/render/target/ScreenRender.h>
#include <api/setup/GraphicService.h>
#include <api/helper/ImageHelper.h>
#include <api/render/GenericRenderer.h>
#include <api/capture/CaptureService.h>

namespace urchin {

    ScreenRender::ScreenRender(std::string name, DepthAttachmentType depthAttachmentType) :
            RenderTarget(std::move(name), depthAttachmentType),
            verticalSyncEnabled(true),
            vkImageIndex(std::numeric_limits<uint32_t>::max()),
            currentFrameIndex(0) {

    }

    ScreenRender::~ScreenRender() {
        if (isInitialized) {
            Logger::instance().logWarning("Screen render not cleanup before destruction");
            ScreenRender::cleanup();
        }
    }

    void ScreenRender::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "scrRenderInit");
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
            destroyImageViews();
            swapChainHandler.cleanup();
            clearValues.clear();

            isInitialized = false;
        }
    }

    void ScreenRender::onResize() {
        unsigned int sceneWidth;
        unsigned int sceneHeight;
        GraphicService::instance().getFramebufferSizeRetriever()->getFramebufferSizeInPixel(sceneWidth, sceneHeight);

        if (sceneWidth > 1 && sceneHeight > 1) { //size is generally invalid when window is minimized on Windows
            cleanup();
            initialize();
        }
    }

    void ScreenRender::updateVerticalSync(bool verticalSyncEnabled) {
        if (this->verticalSyncEnabled != verticalSyncEnabled) {
            ScopeProfiler sp(Profiler::graphic(), "upVertSync");
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

    std::size_t ScreenRender::hasOutputTextureWithContentToLoad() const {
        return false;
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
        swapChainImageViews.clear();
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
                throw std::runtime_error("Failed to create image available semaphore with error code '" + std::to_string(imageAvailableSemaphoreResult) + "' on render target: " + getName());
            }

            VkResult renderFinishedSemaphoreResult = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
            if (renderFinishedSemaphoreResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create render finished semaphore with error code '" + std::to_string(renderFinishedSemaphoreResult) + "' on render target: " + getName());
            }

            VkResult fenceResult = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &commandBufferFences[i]);
            if (fenceResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create fences with error code '" + std::to_string(fenceResult) + "' on render target: " + getName());
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

    void ScreenRender::render(std::uint64_t frameIndex, unsigned int renderTargetUsageCount) { //TODO review name
        ScopeProfiler sp(Profiler::graphic(), "screenRender");

        assert(renderTargetUsageCount == 0);
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        //Fence (CPU-GPU sync) to wait completion of vkQueueSubmit for the frame 'currentFrameIndex'.
        VkResult resultWaitForFences = vkWaitForFences(logicalDevice, 1, &commandBufferFences[currentFrameIndex], VK_TRUE, UINT64_MAX);
        if (resultWaitForFences != VK_SUCCESS && resultWaitForFences != VK_TIMEOUT) {
            throw std::runtime_error("Failed to wait for fence with error code '" + std::to_string(resultWaitForFences) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }

        VkResult resultAcquireImage = vkAcquireNextImageKHR(logicalDevice, swapChainHandler.getSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrameIndex], VK_NULL_HANDLE, &vkImageIndex);
        if (resultAcquireImage == VK_ERROR_OUT_OF_DATE_KHR) {
            onResize();
            return;
        } else if (resultAcquireImage != VK_SUCCESS && resultAcquireImage != VK_SUBOPTIMAL_KHR /* Continue with suboptimal image because already acquired */) {
            throw std::runtime_error("Failed to acquire swap chain image with error code '" + std::to_string(resultAcquireImage) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }

        updateGraphicData(vkImageIndex);
        updateCommandBuffers(vkImageIndex, clearValues);

        if (imagesFences[vkImageIndex] != VK_NULL_HANDLE) {
            //Fence (CPU-GPU sync) to wait if a previous frame is using this image. Useful in 2 cases:
            // 1) MAX_CONCURRENT_FRAMES > swapChainHandler.getSwapChainImages().size()
            // 2) Acquired images from swap chain are returned out-of-order
            resultWaitForFences = vkWaitForFences(logicalDevice, 1, &imagesFences[vkImageIndex], VK_TRUE, UINT64_MAX);
            if (resultWaitForFences != VK_SUCCESS && resultWaitForFences != VK_TIMEOUT) {
                throw std::runtime_error("Failed to wait for fence with error code '" + std::to_string(resultWaitForFences) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
            }
        }
        imagesFences[vkImageIndex] = commandBufferFences[currentFrameIndex]; //mark the image as now being in use by this frame

        //Semaphores (GPU-GPU sync) to wait command buffers execution before present the image.
        std::array<VkSemaphore, 1> queuePresentWaitSemaphores = {renderFinishedSemaphores[currentFrameIndex]};
        //Semaphores (GPU-GPU sync) to wait image available before executing command buffers.
        auto waitSemaphore = WaitSemaphore{imageAvailableSemaphores[currentFrameIndex], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        configureWaitSemaphore(frameIndex, submitInfo, waitSemaphore);
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[vkImageIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = queuePresentWaitSemaphores.data();

        VkResult resultResetFences = vkResetFences(logicalDevice, 1, &commandBufferFences[currentFrameIndex]);
        if (resultResetFences != VK_SUCCESS) {
            throw std::runtime_error("Failed to reset fences with error code '" + std::to_string(resultResetFences) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }
        VkResult resultQueueSubmit = vkQueueSubmit(GraphicService::instance().getQueues().getGraphicsQueue(), 1, &submitInfo, commandBufferFences[currentFrameIndex]);
        if (resultQueueSubmit != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit queue with error code '" + std::to_string(resultQueueSubmit) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }

        std::array<VkSwapchainKHR, 1> swapChains = {swapChainHandler.getSwapChain()};
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = queuePresentWaitSemaphores.data();
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains.data();
        presentInfo.pImageIndices = &vkImageIndex;
        presentInfo.pResults = nullptr;

        VkResult queuePresentResult = vkQueuePresentKHR(GraphicService::instance().getQueues().getPresentationQueue(), &presentInfo);
        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR) {
            onResize();
        } else if (queuePresentResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to acquire swap chain image with error code '" + std::to_string(resultAcquireImage) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }

        currentFrameIndex = (currentFrameIndex + 1) % MAX_CONCURRENT_FRAMES;
    }

    bool ScreenRender::needCommandBufferRefresh(std::size_t /*cmdBufferIndex*/) const {
        //Always return true for the following reasons:
        // - Command buffer refresh is almost always required due to renders make dirty by the UIRenderer
        // - Determine when a refresh is required for a specific command buffer is not easy. Indeed, a dirty render would require a refresh of the current command
        //   buffer but also a refresh of the others command buffers in the next frames.
        return true;
    }

    void ScreenRender::waitCommandBuffersIdle() const {
        ScopeProfiler sp(Profiler::graphic(), "waitCmdBufIdle");

        for (unsigned int frameIndex = 0; frameIndex < MAX_CONCURRENT_FRAMES; ++frameIndex) {
            if (frameIndex != currentFrameIndex) { //current command buffer already idle due to 'vkWaitForFences' previously executed in 'render' method
                //fence (CPU-GPU sync) to wait completion of vkQueueSubmit for the frame 'frameIndex'
                VkResult result = vkWaitForFences(GraphicService::instance().getDevices().getLogicalDevice(), 1, &commandBufferFences[frameIndex], VK_TRUE, UINT64_MAX);
                if (result != VK_SUCCESS && result != VK_TIMEOUT) {
                    throw std::runtime_error("Failed to wait for fence with error code '" + std::to_string(result) + "' on render target: " + getName());
                }
            }
        }
    }

}
