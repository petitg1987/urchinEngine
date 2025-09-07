#include "libs/vma/vk_mem_alloc.h"
#include "MemoryUsage.h"
#include "graphics/api/vulkan/setup/GraphicsSetupService.h"

namespace urchin {

    MemoryUsage::MemoryUsage() :
            memProperties({}),
            budgets(std::make_unique<std::array<VmaBudget, VK_MAX_MEMORY_HEAPS>>()) {
        memProperties.memoryHeapCount = 0;
    }

    void MemoryUsage::onFrameStart() {
        if (memProperties.memoryHeapCount == 0) {
            vkGetPhysicalDeviceMemoryProperties(GraphicsSetupService::instance().getDevices().getPhysicalDevice(), &memProperties);
        }

        if (GraphicsSetupService::instance().getDevices().isMemoryBudgetExtSupported()) {
            vmaGetHeapBudgets(GraphicsSetupService::instance().getAllocator(), budgets->data());
        }
    }

    void MemoryUsage::checkMemoryUsage() const {
        if (GraphicsSetupService::instance().getDevices().isMemoryBudgetExtSupported()) {
            for (std::size_t i = 0; i < memProperties.memoryTypeCount; ++i) {
                VkMemoryPropertyFlags resizeableBarFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

                if ((memProperties.memoryTypes[i].propertyFlags & resizeableBarFlags) == resizeableBarFlags) { //resizeable bar memory
                    uint32_t heapIndex = memProperties.memoryTypes[i].heapIndex;
                    double usagePercentage = (double)(*budgets)[heapIndex].usage / (double)(*budgets)[heapIndex].budget;
                    if (usagePercentage > CRITICAL_RESIZEABLE_BAR_MEMORY_PERCENTAGE_USAGE) [[unlikely]] {
                        logCriticalMemoryUsage("resizeable bar memory", (*budgets)[heapIndex]);
                    }
                } else if (memProperties.memoryTypes[i].propertyFlags == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) { //VRAM (discrete/stand-alone graphic cards only ?)
                    uint32_t heapIndex = memProperties.memoryTypes[i].heapIndex;
                    double usagePercentage = (double)(*budgets)[heapIndex].usage / (double)(*budgets)[heapIndex].budget;
                    if (usagePercentage > CRITICAL_VRAM_PERCENTAGE_USAGE) [[unlikely]] {
                        logCriticalMemoryUsage("VRAM", (*budgets)[heapIndex]);
                    }
                }
            }
        }
    }

    void MemoryUsage::logCriticalMemoryUsage(const std::string& memoryTypeName, const VmaBudget& budget) const {
        static unsigned int numErrorsLogged = 0;
        if (numErrorsLogged < MAX_ERRORS_LOG) [[unlikely]] {
            Logger::instance().logWarning("Critical " + memoryTypeName + " usage: " + std::to_string(budget.usage) + "/" + std::to_string(budget.budget) + " bytes");
            numErrorsLogged++;
        }
    }

    uint64_t MemoryUsage::getUsedMemory() const {
        uint64_t usedMemory = 0;
        if (GraphicsSetupService::instance().getDevices().isMemoryBudgetExtSupported()) {
            for (std::size_t i = 0; i < memProperties.memoryHeapCount; ++i) {
                if (memProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    usedMemory += (*budgets)[i].usage;
                }
            }
        }
        return usedMemory;
    }

    uint64_t MemoryUsage::getTotalMemory() const {
        uint64_t totalMemory = 0;
        if (GraphicsSetupService::instance().getDevices().isMemoryBudgetExtSupported()) {
            for (std::size_t i = 0; i < memProperties.memoryHeapCount; ++i) {
                if (memProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    totalMemory += (*budgets)[i].budget;
                }
            }
        }
        return totalMemory;
    }

}
