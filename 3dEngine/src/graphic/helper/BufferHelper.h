#ifndef URCHINENGINE_BUFFERHELPER_H
#define URCHINENGINE_BUFFERHELPER_H

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

#endif
