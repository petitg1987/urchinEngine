#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <array>

struct VmaBudget;

namespace urchin {

    class MemoryUsage {
        public:
            MemoryUsage();

            void onFrameStart();

            void checkMemoryUsage() const;

            uint64_t getUsedMemory() const;
            uint64_t getTotalMemory() const;

        private:
            void logCriticalMemoryUsage(const std::string&, const VmaBudget&) const;

            static constexpr unsigned int MAX_ERRORS_LOG = 5;
            static constexpr double CRITICAL_RESIZEABLE_BAR_MEMORY_PERCENTAGE_USAGE = 0.99f;
            static constexpr double CRITICAL_VRAM_PERCENTAGE_USAGE = 0.95f;

            VkPhysicalDeviceMemoryProperties memProperties;
            std::unique_ptr<std::array<VmaBudget, VK_MAX_MEMORY_HEAPS>> budgets;
    };

}
