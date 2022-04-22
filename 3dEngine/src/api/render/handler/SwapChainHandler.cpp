#include <cassert>
#include <string>
#include <stdexcept>
#include <algorithm>

#include <api/setup/handler/QueueHandler.h>
#include <api/setup/GraphicService.h>
#include <api/render/handler/SwapChainHandler.h>

namespace urchin {

    SwapChainHandler::SwapChainHandler() :
            isInitialized(false),
            swapChain(nullptr),
            imageFormat(VK_FORMAT_UNDEFINED),
            swapChainExtent({}) {

    }

    SwapChainHandler::~SwapChainHandler() {
        if (isInitialized) {
            Logger::instance().logWarning("Swap chain handler not cleanup before destruction");
            cleanup();
        }
    }

    void SwapChainHandler::initialize(bool verticalSyncEnabled) {
        assert(!isInitialized);
        auto physicalDevice = GraphicService::instance().getDevices().getPhysicalDevice();
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        std::pair<VkFormat, VkColorSpaceKHR> surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes, verticalSyncEnabled);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        uint32_t imageCount = std::max(swapChainSupport.capabilities.minImageCount + 1, 3u /* triple buffering minimum */);
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = GraphicService::getSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.first;
        createInfo.imageColorSpace = surfaceFormat.second;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        //enable transfer source on swap chain images: required for screenshots
        if (swapChainSupport.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
            createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }

        std::array<uint32_t, 2> queueFamilyIndices = {GraphicService::instance().getQueues().getGraphicsQueueFamily(),
                                                      GraphicService::instance().getQueues().getPresentationQueueFamily()};

        if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
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

        VkResult resultCreate = vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain);
        if (resultCreate != VK_SUCCESS) {
            throw std::runtime_error("Failed to create swap chain with error code: " + std::to_string(resultCreate));
        }

        VkResult resultGetImages = vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
        if (resultGetImages != VK_SUCCESS && resultGetImages != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to get swap chain images with error code: " + std::to_string(resultCreate));
        }
        swapChainImages.resize(imageCount);
        resultGetImages = vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());
        if (resultGetImages != VK_SUCCESS && resultGetImages != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to get swap chain images with error code: " + std::to_string(resultCreate));
        }
        imageFormat = surfaceFormat.first;
        swapChainExtent = extent;

        isInitialized = true;
    }

    void SwapChainHandler::cleanup() {
        assert(isInitialized);

        vkDestroySwapchainKHR(GraphicService::instance().getDevices().getLogicalDevice(), swapChain, nullptr);

        isInitialized = false;
    }

    VkSwapchainKHR SwapChainHandler::getSwapChain() const {
        assert(isInitialized);
        return swapChain;
    }

    std::span<VkImage const> SwapChainHandler::getSwapChainImages() const {
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
        VkResult resultSurfaceCapabilities = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, GraphicService::getSurface(), &details.capabilities);
        if (resultSurfaceCapabilities != VK_SUCCESS) {
            throw std::runtime_error("Failed to get surface capabilities with error code: " + std::to_string(resultSurfaceCapabilities));
        }

        uint32_t formatCount;
        VkResult resultSurfaceFormats = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, GraphicService::getSurface(), &formatCount, nullptr);
        if (resultSurfaceFormats != VK_SUCCESS && resultSurfaceFormats != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to get surface formats with error code: " + std::to_string(resultSurfaceFormats));
        }
        if (formatCount != 0) {
            assert(formatCount < 9999); //format count contains wrong value when xcb library is linked statically (bug ?)
            details.formats.resize(formatCount);
            resultSurfaceFormats = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, GraphicService::getSurface(), &formatCount, details.formats.data());
            if (resultSurfaceFormats != VK_SUCCESS && resultSurfaceFormats != VK_INCOMPLETE) {
                throw std::runtime_error("Failed to get surface formats with error code: " + std::to_string(resultSurfaceFormats));
            }
        }

        uint32_t presentModeCount;
        VkResult resultSurfacePresentMode = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, GraphicService::getSurface(), &presentModeCount, nullptr);
        if (resultSurfacePresentMode != VK_SUCCESS && resultSurfacePresentMode != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to get surface present modes with error code: " + std::to_string(resultSurfacePresentMode));
        }
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            resultSurfacePresentMode = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, GraphicService::getSurface(), &presentModeCount, details.presentModes.data());
            if (resultSurfacePresentMode != VK_SUCCESS && resultSurfacePresentMode != VK_INCOMPLETE) {
                throw std::runtime_error("Failed to get surface present modes with error code: " + std::to_string(resultSurfacePresentMode));
            }
        }

        return details;
    }

    std::pair<VkFormat, VkColorSpaceKHR> SwapChainHandler::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const {
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
            return std::make_pair(VK_FORMAT_B8G8R8A8_UNORM, availableFormats[0].colorSpace);
        }

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return std::make_pair(availableFormat.format, availableFormat.colorSpace);
            }
        }

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
                return std::make_pair(availableFormat.format, availableFormat.colorSpace);
            }
        }

        return std::make_pair(availableFormats[0].format, availableFormats[0].colorSpace);
    }

    VkPresentModeKHR SwapChainHandler::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, bool verticalSyncEnabled) const {
        if (!verticalSyncEnabled) {
            if (std::ranges::find(availablePresentModes, VK_PRESENT_MODE_MAILBOX_KHR) != availablePresentModes.end()) {
                return VK_PRESENT_MODE_MAILBOX_KHR;
            } else if (std::ranges::find(availablePresentModes, VK_PRESENT_MODE_IMMEDIATE_KHR) != availablePresentModes.end()) {
                return VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChainHandler::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            unsigned int widthInPixel;
            unsigned int heightInPixel;
            GraphicService::instance().getFramebufferSizeRetriever()->getFramebufferSizeInPixel(widthInPixel, heightInPixel);

            VkExtent2D actualExtent = {widthInPixel, heightInPixel};
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

}
