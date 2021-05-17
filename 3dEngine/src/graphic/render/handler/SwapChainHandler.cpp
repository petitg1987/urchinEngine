#include <cassert>
#include <string>
#include <stdexcept>
#include <algorithm>

#include "graphic/setup/handler/QueueHandler.h"
#include "graphic/setup/GraphicService.h"
#include "SwapChainHandler.h"

namespace urchin {

    SwapChainHandler::SwapChainHandler() :
            isInitialized(false),
            swapChain(nullptr),
            imageFormat(VK_FORMAT_UNDEFINED),
            swapChainExtent({}) {

    }

    SwapChainHandler::~SwapChainHandler() {
        if (isInitialized) {
            Logger::instance()->logWarning("Swap chain handler not cleanup before destruction");
            cleanup();
        }
    }

    void SwapChainHandler::initialize(bool verticalSyncEnabled) {
        assert(!isInitialized);
        auto physicalDevice = GraphicService::instance()->getDevices().getPhysicalDevice();
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes, verticalSyncEnabled);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = GraphicService::getSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = {GraphicService::instance()->getQueues().getGraphicsQueueFamily(),
                                         GraphicService::instance()->getQueues().getPresentationQueueFamily()};

        if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //use alpha channel for blending with other windows in the window system (always opaque)
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create swap chain with error code: " + std::to_string(result));
        }

        vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());
        imageFormat = surfaceFormat.format;
        swapChainExtent = extent;

        isInitialized = true;
    }

    void SwapChainHandler::cleanup() {
        assert(isInitialized);

        vkDestroySwapchainKHR(GraphicService::instance()->getDevices().getLogicalDevice(), swapChain, nullptr);

        isInitialized = false;
    }

    VkSwapchainKHR SwapChainHandler::getSwapChain() const {
        assert(isInitialized);
        return swapChain;
    }

    const std::vector<VkImage>& SwapChainHandler::getSwapChainImages() const {
        assert(isInitialized);
        return swapChainImages;
    }

    VkFormat SwapChainHandler::getImageFormat() const {
        assert(isInitialized);
        return imageFormat;
    }

    VkExtent2D SwapChainHandler::getSwapChainExtent() const {
        assert(isInitialized);
        return swapChainExtent;
    }

    SwapChainSupportDetails SwapChainHandler::querySwapChainSupport(VkPhysicalDevice physicalDevice) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, GraphicService::getSurface(), &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, GraphicService::getSurface(), &formatCount, nullptr);
        if (formatCount != 0) {
            assert(formatCount < 9999); //format count contains wrong value when xcb library is linked statically (bug ?)
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, GraphicService::getSurface(), &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, GraphicService::getSurface(), &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, GraphicService::getSurface(), &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR SwapChainHandler::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR SwapChainHandler::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, bool verticalSyncEnabled) {
        if (verticalSyncEnabled) {
            for (const auto& availablePresentMode : availablePresentModes) {
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    return availablePresentMode;
                }
            }
            return VK_PRESENT_MODE_FIFO_KHR;
        }
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    VkExtent2D SwapChainHandler::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            int widthInPixel, heightInPixel;
            GraphicService::instance()->getFramebufferSizeRetriever()->getFramebufferSizeInPixel(widthInPixel, heightInPixel);

            VkExtent2D actualExtent = {(uint32_t)widthInPixel, (uint32_t)heightInPixel};
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

}
