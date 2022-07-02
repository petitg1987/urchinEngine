#include <graphics/api/vulkan/helper/ImageHelper.h>
#include <libs/vma/vk_mem_alloc.h>
#include <graphics/api/vulkan/setup/VulkanService.h>
#include <graphics/api/vulkan/helper/MemoryHelper.h>
#include "DebugLabelHelper.h"

namespace urchin {

    /**
     * @param imageMemory [out] Device memory for the image
     */
    VkImage ImageHelper::createImage(const std::string& imageName, uint32_t width, uint32_t height, uint32_t layer, uint32_t mipLevels, bool isCubeMap, VkFormat format, VkImageTiling tiling,
                                     VkImageUsageFlags usage, VmaAllocation& imageMemory) {
        auto logicalDevice = VulkanService::instance().getDevices().getLogicalDevice();

        checkFormatSupport(format, tiling, usage);

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent = {width, height, 1};
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = layer;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (isCubeMap) {
            assert(layer == 6);
            imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }

        VkImage image;
        VkResult result = vkCreateImage(logicalDevice, &imageInfo, nullptr, &image);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image " + imageName + " of size " + std::to_string(width) + "x" + std::to_string(height) + " and format " + std::to_string(format) + " with error code: " + std::to_string(result));
        }
        DebugLabelHelper::nameObject(DebugLabelHelper::IMAGE, image, imageName);

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = (tiling == VK_IMAGE_TILING_OPTIMAL) ? VMA_MEMORY_USAGE_GPU_ONLY : VMA_MEMORY_USAGE_GPU_TO_CPU; //assume that tiling no optimal is only used for GPU to CPU transfers
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocationInfo allocInfo{};
        allocInfo.size = memRequirements.size;
        allocInfo.memoryType = MemoryHelper::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        result = vmaAllocateMemoryForImage(VulkanService::instance().getAllocator(), image, &allocCreateInfo, &imageMemory, &allocInfo);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate image memory for image " + imageName + " of size " + std::to_string(width) + "x" + std::to_string(height) + " and format " + std::to_string(format) + " with error code: " + std::to_string(result));
        }

        result = vmaBindImageMemory(VulkanService::instance().getAllocator(), imageMemory, image);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to bind image memory for image " + imageName + " of size " + std::to_string(width) + "x" + std::to_string(height) + " and format " + std::to_string(format) + " with error code: " + std::to_string(result));
        }

        return image;
    }

    VkImageView ImageHelper::createImageView(VkImage image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t layerCount, uint32_t mipLevels) {
        auto logicalDevice = VulkanService::instance().getDevices().getLogicalDevice();

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = type;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = layerCount;

        VkImageView imageView;
        VkResult result = vkCreateImageView(logicalDevice, &viewInfo, nullptr, &imageView);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view (format: " + std::to_string(format) + ") with error code: " + std::to_string(result));
        }

        return imageView;
    }

    void ImageHelper::checkFormatSupport(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage) {
        VkFormatFeatureFlags features = usageFlagToFeatureFlag(usage);

        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(VulkanService::instance().getDevices().getPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) != features) {
            throw std::runtime_error("Unsupported format " + std::to_string(format) + " for tiling linear and features " + std::to_string(features));
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) != features) {
            throw std::runtime_error("Unsupported format " + std::to_string(format) + " for tiling optimal and features " + std::to_string(features));
        }
    }

    VkFormatFeatureFlags ImageHelper::usageFlagToFeatureFlag(VkImageUsageFlags usage) {
        if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) { //contain depth stencil attachment
            return VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        } else if (usage == VK_IMAGE_USAGE_TRANSFER_DST_BIT) { //only for transfer destination
            return VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
        }
        return VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    }

}
