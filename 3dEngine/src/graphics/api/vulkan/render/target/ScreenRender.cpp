#include <stdexcept>
#include <utility>
#include <thread>
#include <libs/vkenum/vk_enum.h>

#include <graphics/api/vulkan/render/target/ScreenRender.h>
#include <graphics/api/vulkan/render/target/OffscreenRender.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/api/vulkan/helper/ImageHelper.h>
#include <graphics/api/vulkan/render/GenericRenderer.h>
#include <graphics/api/vulkan/capture/CaptureService.h>

namespace urchin {

    ScreenRender::ScreenRender(std::string name, bool isTestMode, DepthAttachmentType depthAttachmentType) :
            RenderTarget(std::move(name), isTestMode, depthAttachmentType),
            verticalSyncEnabled(true),
            vkImageIndex(std::numeric_limits<uint32_t>::max()),
            currentFrameIndex(0) {

    }

    ScreenRender::~ScreenRender() {
        if (isInitialized()) {
            Logger::instance().logWarning("Screen render not cleanup before destruction");
            ScreenRender::cleanup();
        }
    }

    void ScreenRender::initialize() {
        ScopeProfiler sp(Profiler::graphic(), "scrRenderInit");
        assert(!isInitialized());

        initializeClearValues();
        swapChainHandler.initialize(verticalSyncEnabled);
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createCommandPool();
        createCommandBuffers();
        createSyncObjects();

        initializeProcessors();

        setInitialized(true);
    }

    void ScreenRender::cleanup() {
        if (isInitialized()) {
            VkResult result = vkDeviceWaitIdle(GraphicsSetupService::instance().getDevices().getLogicalDevice());
            if (result != VK_SUCCESS) {
                Logger::instance().logError("Failed to wait for device idle with error code '" + std::string(string_VkResult(result)) + "' on render target: " + getName());
            }

            cleanupProcessors();

            destroySyncObjects();
            destroyCommandBuffersAndPool();
            destroyFramebuffers();
            destroyDepthResources();
            destroyRenderPass();
            destroyImageViews();
            swapChainHandler.cleanup();
            clearValues.clear();

            setInitialized(false);
        }
    }

    void ScreenRender::onResize() {
        unsigned int sceneWidth;
        unsigned int sceneHeight;
        GraphicsSetupService::instance().getFramebufferSizeRetriever()->getFramebufferSizeInPixel(sceneWidth, sceneHeight);

        if (sceneWidth != 0 && sceneHeight != 0) { //size is generally invalid when window is minimized on Windows
            cleanup();
            initialize();
        }
    }

    void ScreenRender::updateVerticalSync(bool verticalSyncEnabled) {
        if (this->verticalSyncEnabled != verticalSyncEnabled) {
            ScopeProfiler sp(Profiler::graphic(), "upVertSync");
            this->verticalSyncEnabled = verticalSyncEnabled;

            if (isInitialized()) {
                cleanup();
                initialize();
            }
        }
    }

    bool ScreenRender::isVerticalSyncEnabled() const {
        return verticalSyncEnabled;
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
        CaptureService::instance().takeCapture(filename, srcImage, swapChainHandler.getImageFormat(), getWidth(), getHeight(), dstWidth, dstHeight);
    }

    void ScreenRender::initializeClearValues() {
        if (hasDepthAttachment()) {
            VkClearValue clearDepth{};
            clearDepth.depthStencil = {1.0f, 0};
            clearValues.emplace_back(clearDepth);
        }
    }

    void ScreenRender::createImageViews() {
        swapChainImageViews.resize(swapChainHandler.getSwapChainImages().size());

        for (size_t i = 0; i < swapChainHandler.getSwapChainImages().size(); i++) {
            VkImage image = swapChainHandler.getSwapChainImages()[i];
            swapChainImageViews[i] = ImageHelper::createImageViews(image, VK_IMAGE_VIEW_TYPE_2D, swapChainHandler.getImageFormat(), VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, false)[0];
        }
    }

    void ScreenRender::destroyImageViews() {
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(GraphicsSetupService::instance().getDevices().getLogicalDevice(), imageView, nullptr);
        }
        swapChainImageViews.clear();
    }

    void ScreenRender::createRenderPass() {
        std::vector<VkAttachmentDescription2> attachments;
        uint32_t attachmentIndex = 0;

        VkAttachmentReference2 depthAttachmentRef{};
        depthAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
        if (hasDepthAttachment()) {
            attachments.emplace_back(buildDepthAttachment(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
            depthAttachmentRef.attachment = attachmentIndex++;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        std::vector<VkAttachmentReference2> colorAttachmentRefs;
        attachments.emplace_back(buildAttachment(swapChainHandler.getImageFormat(), false, false, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));
        VkAttachmentReference2 colorAttachmentRef{};
        colorAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
        colorAttachmentRef.attachment = attachmentIndex;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentRefs.push_back(colorAttachmentRef);

        RenderTarget::createRenderPass(depthAttachmentRef, colorAttachmentRefs, attachments);
    }

    void ScreenRender::createFramebuffers() {
        for (auto& swapChainImageView : swapChainImageViews) {
            std::vector<std::vector<VkImageView>> attachments;
            attachments.resize(1); //1 layer for screen rendering

            std::size_t layerIndex = 0;
            attachments[layerIndex].reserve(hasDepthAttachment() ? 2 : 1);
            if (hasDepthAttachment()) {
                attachments[layerIndex].emplace_back(getDepthTexture()->getImageView());
            }
            attachments[layerIndex].emplace_back(swapChainImageView);

            RenderTarget::addFramebuffers(attachments);
        }
    }

    void ScreenRender::createSyncObjects() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

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
                throw std::runtime_error("Failed to create image available semaphore with error code '" + std::string(string_VkResult(imageAvailableSemaphoreResult)) + "' on render target: " + getName());
            }

            VkResult renderFinishedSemaphoreResult = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
            if (renderFinishedSemaphoreResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create render finished semaphore with error code '" + std::string(string_VkResult(renderFinishedSemaphoreResult)) + "' on render target: " + getName());
            }

            VkResult fenceResult = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &commandBufferFences[i]);
            if (fenceResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create fences with error code '" + std::string(string_VkResult(fenceResult)) + "' on render target: " + getName());
            }
        }
    }

    void ScreenRender::destroySyncObjects() {
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        imagesFences.clear();
        for (std::size_t i = 0; i < MAX_CONCURRENT_FRAMES; i++) {
            vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(logicalDevice, commandBufferFences[i], nullptr);
        }
    }

    void ScreenRender::render(std::uint32_t frameIndex, unsigned int numDependenciesToOutputs) {
        ScopeProfiler sp(Profiler::graphic(), "screenRender");
        auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

        //Fence (CPU-GPU sync) to wait completion of vkQueueSubmit2 for the frame 'currentFrameIndex'.
        VkResult resultWaitForFences = vkWaitForFences(logicalDevice, 1, &commandBufferFences[currentFrameIndex], VK_TRUE, UINT64_MAX);
        if (resultWaitForFences != VK_SUCCESS && resultWaitForFences != VK_TIMEOUT) {
            throw std::runtime_error("Failed to wait for fence with error code '" + std::string(string_VkResult(resultWaitForFences)) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }

        if (numDependenciesToOutputs != 0) {
            throw std::runtime_error("No dependencies to outputs expected on screen render target: " + getName() + "/" + std::to_string(frameIndex));
        }

        VkResult resultAcquireImage = vkAcquireNextImageKHR(logicalDevice, swapChainHandler.getSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrameIndex], VK_NULL_HANDLE, &vkImageIndex);
        if (resultAcquireImage == VK_ERROR_OUT_OF_DATE_KHR) { //after window resize (never had the case !) or when window is minimized with Alt+Tab or Win+D
            onResize();
            std::ranges::for_each(getOffscreenRenderDependencies(), [frameIndex](OffscreenRender* ord){ ord->markSubmitSemaphoreUnused(frameIndex); });
            return;
        } else if (resultAcquireImage != VK_SUCCESS && resultAcquireImage != VK_SUBOPTIMAL_KHR /* Continue with suboptimal image because already acquired */) {
            throw std::runtime_error("Failed to acquire swap chain image with error code '" + std::string(string_VkResult(resultAcquireImage)) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }

        updatePipelineProcessorData(vkImageIndex);
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
        VkSemaphoreSubmitInfo semaphoreSubmitInfo{};
        semaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        semaphoreSubmitInfo.semaphore = renderFinishedSemaphores[currentFrameIndex];
        semaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        semaphoreSubmitInfo.deviceIndex = 0;

        //Semaphores (GPU-GPU sync) to wait image available before executing command buffers.
        VkSemaphoreSubmitInfo imageAvailableSemaphoreSubmitInfo{};
        imageAvailableSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        imageAvailableSemaphoreSubmitInfo.semaphore = imageAvailableSemaphores[currentFrameIndex];
        imageAvailableSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageAvailableSemaphoreSubmitInfo.deviceIndex = 0;

        VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
        commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        commandBufferSubmitInfo.commandBuffer = getCommandBuffer(vkImageIndex);
        commandBufferSubmitInfo.deviceMask = 0;

        VkSubmitInfo2 submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        configureWaitSemaphore(frameIndex, submitInfo, imageAvailableSemaphoreSubmitInfo);
        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &commandBufferSubmitInfo;
        submitInfo.signalSemaphoreInfoCount = 1;
        submitInfo.pSignalSemaphoreInfos = &semaphoreSubmitInfo;

        VkResult resultResetFences = vkResetFences(logicalDevice, 1, &commandBufferFences[currentFrameIndex]);
        if (resultResetFences != VK_SUCCESS) {
            throw std::runtime_error("Failed to reset fences with error code '" + std::string(string_VkResult(resultResetFences)) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }
        VkResult resultQueueSubmit = vkQueueSubmit2(GraphicsSetupService::instance().getQueues().getGraphicsAndComputeQueue(), 1, &submitInfo, commandBufferFences[currentFrameIndex]);
        if (resultQueueSubmit != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit queue with error code '" + std::string(string_VkResult(resultQueueSubmit)) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
        }

        std::array<VkSwapchainKHR, 1> swapChains = {swapChainHandler.getSwapChain()};
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrameIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains.data();
        presentInfo.pImageIndices = &vkImageIndex;
        presentInfo.pResults = nullptr;

        VkResult queuePresentResult = vkQueuePresentKHR(GraphicsSetupService::instance().getQueues().getPresentationQueue(), &presentInfo);
        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR) {
            onResize();
        } else if (queuePresentResult == VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT) { //shouldn't happen but the error has appeared before on some AMD GPUs
            static unsigned int numErrorsLogged = 0;
            if (numErrorsLogged++ < MAX_ERRORS_LOG) {
                Logger::instance().logError("Error when queuing an image for presentation: VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT");
            }
            onResize();
        } else if (queuePresentResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to queue an image for presentation with error code '" + std::string(string_VkResult(queuePresentResult)) + "' on render target: " + getName() + "/" + std::to_string(frameIndex));
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
                //fence (CPU-GPU sync) to wait completion of vkQueueSubmit2 for the frame 'frameIndex'
                VkResult result = vkWaitForFences(GraphicsSetupService::instance().getDevices().getLogicalDevice(), 1, &commandBufferFences[frameIndex], VK_TRUE, UINT64_MAX);
                if (result != VK_SUCCESS && result != VK_TIMEOUT) {
                    throw std::runtime_error("Failed to wait for fence with error code '" + std::string(string_VkResult(result)) + "' on render target: " + getName());
                }
            }
        }
    }

}
