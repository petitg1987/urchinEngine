#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace urchin {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class SwapChainHandler {
        public:
            SwapChainHandler();
            ~SwapChainHandler();

            void initialize(bool);
            void cleanup();

            VkSwapchainKHR getSwapChain() const;
            std::span<VkImage const> getSwapChainImages() const;
            VkFormat getImageFormat() const;
            VkExtent2D getSwapChainExtent() const;

            static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice);

        private:
            std::pair<VkFormat, VkColorSpaceKHR> chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&) const;
            VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&, bool) const;
            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&) const;

            bool isInitialized;

            VkSwapchainKHR swapChain;
            std::vector<VkImage> swapChainImages;
            VkFormat imageFormat;
            VkExtent2D swapChainExtent;
    };

}
