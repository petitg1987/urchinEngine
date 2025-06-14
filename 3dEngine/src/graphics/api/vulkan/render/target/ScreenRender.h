#pragma once

#include <vector>
#include <array>
#include <vulkan/vulkan.h>

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/handler/SwapChainHandler.h>

namespace urchin {

    class ScreenRender final : public RenderTarget {
        public:
            ScreenRender(std::string, bool, DepthAttachmentType);
            ~ScreenRender() override;

            void initialize() override;
            void cleanup() override;

            void onResize();

            void updateVerticalSync(bool);
            bool isVerticalSyncEnabled() const;

            unsigned int getWidth() const override;
            unsigned int getHeight() const override;
            unsigned int getLayer() const override;
            bool isArrayOutput() const override;
            std::size_t getNumFramebuffer() const override;
            std::size_t getNumColorAttachment() const override;

            void takeScreenshot(const std::string&, unsigned int = 0, unsigned int = 0) const;

            void render(uint32_t, unsigned int) override;

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
            static constexpr unsigned int MAX_ERRORS_LOG = 20;
            std::size_t currentFrameIndex;

            std::array<VkSemaphore, MAX_CONCURRENT_FRAMES> presentCompleteSemaphores;
            std::vector<std::array<VkSemaphore, MAX_CONCURRENT_FRAMES>> renderCompleteSemaphores;
            std::array<VkFence, MAX_CONCURRENT_FRAMES> commandBufferFences;
            std::vector<VkFence> imagesFences;
    };

}
