#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include <api/render/target/RenderTarget.h>
#include <api/render/handler/SwapChainHandler.h>

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
            std::size_t hasOutputTextureWithContentToLoad() const override;

            void takeScreenshot(const std::string&, unsigned int = 0, unsigned int = 0) const;

            void render(unsigned int) override;

        private:
            void initializeClearValues();
            void createImageViews();
            void destroyImageViews();
            void createRenderPass();
            void createFramebuffers();
            void createSyncObjects();
            void destroySyncObjects();

            bool needCommandBufferRefresh(std::size_t) const override;
            void waitCommandBuffersIdle() const override;

            bool verticalSyncEnabled;

            std::vector<VkClearValue> clearValues;
            SwapChainHandler swapChainHandler;
            uint32_t vkImageIndex;
            std::vector<VkImageView> swapChainImageViews;

            static constexpr std::size_t MAX_CONCURRENT_FRAMES = 2;
            std::size_t currentFrameIndex;

            std::vector<VkSemaphore> imageAvailableSemaphores;
            std::vector<VkSemaphore> renderFinishedSemaphores;
            std::vector<VkFence> commandBufferFences;
            std::vector<VkFence> imagesFences;
    };

}
