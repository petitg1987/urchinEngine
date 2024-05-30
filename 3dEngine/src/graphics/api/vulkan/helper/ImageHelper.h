#pragma once

#include <vulkan/vulkan.h>

VK_DEFINE_HANDLE(VmaAllocation)

namespace urchin {

    class ImageHelper {
        public:
            static VkImage createImage(const std::string&, uint32_t, uint32_t, uint32_t, uint32_t, bool, VkFormat, VkImageTiling, VkImageUsageFlags, VmaAllocation&);
            static std::vector<VkImageView> createImageViews(VkImage, VkImageViewType, VkFormat, VkImageAspectFlags, uint32_t, uint32_t, bool);

        private:
            static void checkFormatSupport(VkFormat, VkImageTiling, VkFormatFeatureFlags);
            static VkFormatFeatureFlags usageFlagToFeatureFlag(VkImageUsageFlags);
    };

}
