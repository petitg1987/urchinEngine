#include "ImageHelper.h"
#include "libs/vma/vk_mem_alloc.h"
#include "graphic/setup/GraphicService.h"
#include "graphic/helper/MemoryHelper.h"

namespace urchin {

    /**
     * @param imageMemory [out] Device memory for the image
     */
    VkImage ImageHelper::createImage(uint32_t width, uint32_t height, uint32_t layer, uint32_t mipLevels, bool isCubeMap, VkFormat format, VkImageUsageFlags usage, VmaAllocation& imageMemory) {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
        VkFormatFeatureFlags formatFeature = usageFlagToFeatureFlag(usage);
        checkFormatSupport(format, tiling, formatFeature);

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
        if(isCubeMap) {
            assert(layer == 6);
            imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }

        VkImage image;
        VkResult result = vkCreateImage(logicalDevice, &imageInfo, nullptr, &image);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image of size " + std::to_string(width) + "x" + std::to_string(height) + " and format " + std::to_string(format) + " with error code: " + std::to_string(result));
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocationInfo allocInfo{};
        allocInfo.size = memRequirements.size;
        allocInfo.memoryType = MemoryHelper::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vmaAllocateMemoryForImage(GraphicService::instance()->getAllocator(), image, &allocCreateInfo, &imageMemory, &allocInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate image memory for image of size " + std::to_string(width) + "x" + std::to_string(height) + " with format: " + std::to_string(format));
        }

        vmaBindImageMemory(GraphicService::instance()->getAllocator(), imageMemory, image);

        return image;
    }

    VkImageView ImageHelper::createImageView(VkImage image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t layerCount, uint32_t mipLevels) {
        auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

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
            throw std::runtime_error("Failed to create texture image (format: " + std::to_string(format) + ") with error code: " + std::to_string(result));
        }

        return imageView;
    }

    void ImageHelper::checkFormatSupport(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(GraphicService::instance()->getDevices().getPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) != features) {
            throw std::runtime_error("Unsupported format " + std::to_string(format) + " for tiling linear and features " + std::to_string(features));
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) != features) {
            throw std::runtime_error("Unsupported format " + std::to_string(format) + " for tiling optimal and features " + std::to_string(features));
        }
    }

    VkFormatFeatureFlags ImageHelper::usageFlagToFeatureFlag(VkImageUsageFlags usage) {
        if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        return VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    }

}
