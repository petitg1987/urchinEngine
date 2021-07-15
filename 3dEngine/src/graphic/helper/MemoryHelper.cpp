#include <stdexcept>

#include <graphic/helper/MemoryHelper.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    uint32_t MemoryHelper::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(GraphicService::instance().getDevices().getPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type with type filter: " + std::to_string(typeFilter));
    }

}
