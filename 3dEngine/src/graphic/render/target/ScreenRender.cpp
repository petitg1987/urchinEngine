#include <stdexcept>

#include "ScreenRender.h"
#include "graphic/setup/GraphicService.h"
#include "graphic/helper/ImageHelper.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    //static
    const std::size_t ScreenRender::MAX_CONCURRENT_FRAMES = 2;

    ScreenRender::ScreenRender(DepthAttachmentType depthAttachmentType, bool verticalSyncEnabled) :
            RenderTarget(depthAttachmentType),
            isInitialized(false),
            verticalSyncEnabled(verticalSyncEnabled) {

    }

    ScreenRender::~ScreenRender() {
        if(isInitialized) {
            Logger::instance()->logWarning("Screen render not cleanup before destruction");
            ScreenRender::cleanup();
        }
    }

    void ScreenRender::initialize() {
        assert(!isInitialized);

        RenderTarget::initialize();
        swapChainHandler.initialize(verticalSyncEnabled);
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createCommandPool();
        createCommandBuffers();
        createSyncObjects();

        isInitialized = true;
    }

    void ScreenRender::cleanup() {
        assert(isInitialized);

        vkDeviceWaitIdle(GraphicService::instance()->getDevices().getLogicalDevice());

        destroySyncObjects();
        destroyCommandBuffersAndPool();
        destroyFramebuffers();
        destroyDepthResources();
        destroyRenderPass();
        destroyImageViews();
        swapChainHandler.cleanup();
        RenderTarget::cleanup();

        isInitialized = false;
    }

    unsigned int ScreenRender::getWidth() const {
        return swapChainHandler.getSwapChainExtent().width;
    }

    unsigned int ScreenRender::getHeight() const {
        return swapChainHandler.getSwapChainExtent().height;
    }

    std::size_t ScreenRender::getNumFramebuffer() const {
        return swapChainHandler.getSwapChainImages().size();
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
            vkDestroyImageView(GraphicService::instance()->getDevices().getLogicalDevice(), imageView, nullptr);
        }
    }

    void ScreenRender::createRenderPass() {
        std::vector<VkAttachmentDescription> attachments;

        VkAttachmentReference depthAttachmentRef{};
        if (hasDepthAttachment()) {
            attachments.emplace_back(buildDepthAttachment(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
            depthAttachmentRef.attachment = (uint32_t) attachments.size() - 1;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        attachments.emplace_back(buildAttachment(swapChainHandler.getImageFormat(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = (uint32_t )attachments.size() - 1;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        RenderTarget::createRenderPass(depthAttachmentRef, colorAttachmentRef, attachments);
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
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

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
                throw std::runtime_error("Failed to create image available semaphores with error code: " + std::to_string(imageAvailableSemaphoreResult));
            }

            VkResult renderFinishedSemaphoreResult = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
            if (renderFinishedSemaphoreResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create render finished semaphores with error code: " + std::to_string(renderFinishedSemaphoreResult));
            }

            VkResult fenceResult = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &commandBufferFences[i]);
            if (fenceResult != VK_SUCCESS) {
                throw std::runtime_error("Failed to create fences with error code: " + std::to_string(fenceResult));
            }
        }
    }

    void ScreenRender::destroySyncObjects() {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

        imagesFences.clear();
        for (std::size_t i = 0; i < MAX_CONCURRENT_FRAMES; i++) {
            vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(logicalDevice, commandBufferFences[i], nullptr);
        }
    }

    void ScreenRender::render() {
        assert(!renderers.empty());

        static size_t currentFrameIndex = 0;
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

        //fence to wait completion of vkQueueSubmit of the frame 'currentFrameIndex'
        vkWaitForFences(logicalDevice, 1, &commandBufferFences[currentFrameIndex], VK_TRUE, UINT64_MAX);

        uint32_t vkImageIndex;
        VkResult resultAcquireImage = vkAcquireNextImageKHR(logicalDevice, swapChainHandler.getSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrameIndex], VK_NULL_HANDLE, &vkImageIndex);
        if (resultAcquireImage == VK_ERROR_OUT_OF_DATE_KHR) {
            onResize();
            return;
        } else if(resultAcquireImage != VK_SUCCESS && resultAcquireImage != VK_SUBOPTIMAL_KHR /*Continue with sub optimal image because already acquired */) {
            throw std::runtime_error("Failed to acquire swap chain image with error code: " + std::to_string(resultAcquireImage));
        }

        updateGraphicData(vkImageIndex);
        updateCommandBuffers();

        //fence to wait if a previous frame is using this image (can happen when MAX_CONCURRENT_FRAMES > swap chain images size)
        if (imagesFences[vkImageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(logicalDevice, 1, &imagesFences[vkImageIndex], VK_TRUE, UINT64_MAX);
        }
        imagesFences[vkImageIndex] = commandBufferFences[currentFrameIndex]; //mark the image as now being in use by this frame

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrameIndex]};
        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrameIndex]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[vkImageIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(logicalDevice, 1, &commandBufferFences[currentFrameIndex]); //ensure fences are reset just before use them
        VkResult result = vkQueueSubmit(GraphicService::instance()->getQueues().getGraphicsQueue(), 1, &submitInfo, commandBufferFences[currentFrameIndex]); //vkQueueSubmit is executed when vkAcquireNextImageKHR is complete (thanks to 'waitSemaphores')
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer with error code: " + std::to_string(result));
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = {swapChainHandler.getSwapChain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &vkImageIndex;
        presentInfo.pResults = nullptr;

        //vkQueuePresentKHR is executed when vkQueueSubmit is complete (thanks to 'signalSemaphores')
        VkResult queuePresentResult = vkQueuePresentKHR(GraphicService::instance()->getQueues().getPresentationQueue(), &presentInfo);
        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR) {
            onResize();
        } else if (queuePresentResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to acquire swap chain image with error code: " + std::to_string(resultAcquireImage));
        }

        currentFrameIndex = (currentFrameIndex + 1) % MAX_CONCURRENT_FRAMES;
    }

    void ScreenRender::updateGraphicData(uint32_t frameIndex) {
        for (auto &renderer : renderers) {
            renderer->updateGraphicData(frameIndex);
        }
    }

    void ScreenRender::waitCommandBuffersIdle() const {
        for (unsigned int i = 0; i < MAX_CONCURRENT_FRAMES; ++i) {
            vkWaitForFences(GraphicService::instance()->getDevices().getLogicalDevice(), 1, &commandBufferFences[i], VK_TRUE, UINT64_MAX);
        }
    }

}
