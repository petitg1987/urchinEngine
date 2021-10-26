#pragma once

#include <vulkan/vulkan.h>

namespace urchin {

    class MemoryHelper {
        public:
            static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };

}
