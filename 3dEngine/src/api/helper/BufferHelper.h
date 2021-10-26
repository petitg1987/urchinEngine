#pragma once

#include <vulkan/vulkan.h>

VK_DEFINE_HANDLE(VmaAllocation)

namespace urchin {

    class BufferHelper {
        public:
            static VkBuffer createBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VmaAllocation&);

        private:
            static int vulkanMemoryPropertyFlagsToVma(VkMemoryPropertyFlags);
    };

}
