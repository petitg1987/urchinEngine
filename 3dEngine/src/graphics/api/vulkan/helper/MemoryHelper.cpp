#include <stdexcept>

#include <libs/vma/vk_mem_alloc.h>
#include <graphics/api/vulkan/helper/MemoryHelper.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>

namespace urchin {

    uint32_t MemoryHelper::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(GraphicsSetupService::instance().getDevices().getPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type with type filter: " + std::to_string(typeFilter));
    }

    void MemoryHelper::checkMemoryUsage() {
        if (GraphicsSetupService::instance().getDevices().isMemoryBudgetExtEnabled()) {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(GraphicsSetupService::instance().getDevices().getPhysicalDevice(), &memProperties);

            std::array<VmaBudget, VK_MAX_MEMORY_HEAPS> budgets{};
            vmaGetHeapBudgets(GraphicsSetupService::instance().getAllocator(), budgets.data());

            for (std::size_t i = 0; i < memProperties.memoryTypeCount; ++i) {
                VkMemoryPropertyFlags resizeableBarFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

                if ((memProperties.memoryTypes[i].propertyFlags & resizeableBarFlags) == resizeableBarFlags) { //resizeable bar memory
                    uint32_t heapIndex = memProperties.memoryTypes[i].heapIndex;
                    double usagePercentage = (double) budgets[heapIndex].usage / (double) budgets[heapIndex].budget;
                    if (usagePercentage > CRITICAL_RESIZEABLE_BAR_MEMORY_PERCENTAGE_USAGE) [[unlikely]] {
                        logCriticalMemoryUsage("resizeable bar memory", budgets[heapIndex]);
                    }
                } else if (memProperties.memoryTypes[i].propertyFlags == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) { //VRAM (discrete graphic cards only ?)
                    uint32_t heapIndex = memProperties.memoryTypes[i].heapIndex;
                    double usagePercentage = (double) budgets[heapIndex].usage / (double) budgets[heapIndex].budget;
                    if (usagePercentage > CRITICAL_VRAM_PERCENTAGE_USAGE) [[unlikely]] {
                        logCriticalMemoryUsage("VRAM", budgets[heapIndex]);
                    }
                }
            }
        }
    }

    void MemoryHelper::logCriticalMemoryUsage(const std::string& memoryTypeName, const VmaBudget& budget) {
        static unsigned int numErrorsLogged = 0;
        if (numErrorsLogged < MAX_ERRORS_LOG) [[unlikely]] {
            Logger::instance().logWarning("Critical " + memoryTypeName + " usage: " + std::to_string(budget.usage) + "/" + std::to_string(budget.budget) + " bytes");
            numErrorsLogged++;
        }
    }

}
