#include "BufferHelper.h"
#include <libs/vma/vk_mem_alloc.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    VkBuffer BufferHelper::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VmaAllocation& allocation) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo vmaAllocInfo{};
        vmaAllocInfo.usage = static_cast<VmaMemoryUsage>(vulkanMemoryPropertyFlagsToVma(properties));

        VkBuffer buffer;
        VkResult result = vmaCreateBuffer(GraphicService::instance()->getAllocator(), &bufferInfo, &vmaAllocInfo, &buffer, &allocation, nullptr);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create buffer with error code: " + std::to_string(result));
        }

        return buffer;
    }

    int BufferHelper::vulkanMemoryPropertyFlagsToVma(VkMemoryPropertyFlags properties) {
        if (properties == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            return VMA_MEMORY_USAGE_CPU_ONLY;
        } else if (properties == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            return VMA_MEMORY_USAGE_GPU_ONLY;
        } else {
            throw std::runtime_error("Unknown memory properties: " + std::to_string(properties));
        }
    }

}
