#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/handler/SwapChainHandler.h>

namespace urchin {

    class ScreenRender : public RenderTarget {
        public:
            ScreenRender(std::string, DepthAttachmentType);
            ~ScreenRender() override;

            void initialize() override;
            void cleanup() override;

            void updateVerticalSync(bool);
            void onResize();

            unsigned int getWidth() const override;
            unsigned int getHeight() const override;
            unsigned int getLayer() const override;
            std::size_t getNumFramebuffer() const override;
            std::size_t getNumColorAttachment() const override;

            VkImage getCurrentImage() const;
            VkFormat getImageFormat() const;

            void render() override;

        private:
            void initializeClearValues();
            void createImageViews();
            void destroyImageViews();
            void createRenderPass();
            void createFramebuffers();
            void createSyncObjects();
            void destroySyncObjects();

            void waitCommandBuffersIdle() const override;

            bool isInitialized;
            bool verticalSyncEnabled;

            std::vector<VkClearValue> clearValues;
            SwapChainHandler swapChainHandler;
            uint32_t vkImageIndex;
            std::vector<VkImageView> swapChainImageViews;

            static const std::size_t MAX_CONCURRENT_FRAMES;
            std::vector<VkSemaphore> imageAvailableSemaphores;
            std::vector<VkSemaphore> renderFinishedSemaphores;
            std::vector<VkFence> commandBufferFences;
            std::vector<VkFence> imagesFences;
    };

}
