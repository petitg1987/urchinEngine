#ifndef URCHINENGINE_MEMORYHELPER_H
#define URCHINENGINE_MEMORYHELPER_H

#include <vulkan/vulkan.h>

namespace urchin {

    class MemoryHelper {
        public:
            static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };

}

#endif
