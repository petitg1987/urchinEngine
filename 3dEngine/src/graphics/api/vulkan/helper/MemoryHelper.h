#pragma once

#include <vulkan/vulkan.h>
#include <libs/vma/vk_mem_alloc.h>

namespace urchin {

    class MemoryHelper {
        public:
            static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
            static void checkMemoryUsage();

        private:
            static void logCriticalMemoryUsage(const std::string&, const VmaBudget&);

            static constexpr unsigned int MAX_ERRORS_LOG = 5;
            static constexpr double CRITICAL_RESIZEABLE_BAR_MEMORY_PERCENTAGE_USAGE = 0.99f;
            static constexpr double CRITICAL_VRAM_PERCENTAGE_USAGE = 0.95f;
    };

}
