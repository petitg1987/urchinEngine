#ifndef URCHINENGINE_SWAPCHAINHANDLER_H
#define URCHINENGINE_SWAPCHAINHANDLER_H

#include <vector>
#include <memory>
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
            const std::vector<VkImage>& getSwapChainImages() const;
            VkFormat getImageFormat() const;
            VkExtent2D getSwapChainExtent() const;

            static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice);

        private:
            static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&);
            static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&, bool);
            static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&);

            bool isInitialized;

            VkSwapchainKHR swapChain;
            std::vector<VkImage> swapChainImages;
            VkFormat imageFormat;
            VkExtent2D swapChainExtent;
    };

}

#endif
