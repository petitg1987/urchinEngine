#include <vector>
#include <cassert>
#include <vulkan/vk_enum_string_helper.h>

#include <graphics/api/vulkan/setup/handler/QueueHandler.h>

namespace urchin {

    QueueHandler::QueueHandler() :
            queueFamiliesInitialized(false),
            queuesInitialized(false),
            graphicsQueue(nullptr),
            computeQueue(nullptr),
            presentationQueue(nullptr) {

    }

    void QueueHandler::initializeQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        //try to find a unique queue family for graphic/compute and presentation: better for performance
        uint32_t i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (isGraphicsAndComputeQueueFamily(queueFamily) && isPresentationQueueFamily(i, physicalDevice, surface)) {
                graphicsAndComputeQueueFamily = i;
                presentationQueueFamily = i;
                assert(isAllQueueFamiliesFound());
                break;
            }
            i++;
        }

        //if not found: search different queue families
        if (!isAllQueueFamiliesFound()) {
            i = 0;
            for (const auto& queueFamily : queueFamilies) {
                if (isGraphicsAndComputeQueueFamily(queueFamily)) {
                    graphicsAndComputeQueueFamily = i;
                }
                if (isPresentationQueueFamily(i, physicalDevice, surface)) {
                    presentationQueueFamily = i;
                }

                if (isAllQueueFamiliesFound()) {
                    break;
                }
                i++;
            }
        }

        queueFamiliesInitialized = true;
    }

    bool QueueHandler::isGraphicsAndComputeQueueFamily(VkQueueFamilyProperties queueFamily) const {
        return queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT;
    }

    bool QueueHandler::isPresentationQueueFamily(uint32_t queueFamilyIndex, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) const {
        VkBool32 presentationSupport = false;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &presentationSupport);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to get physics device surface with error code: " + std::string(string_VkResult(result)));
        }
        return presentationSupport;
    }

    void QueueHandler::initializeQueues(VkDevice logicalDevice) {
        vkGetDeviceQueue(logicalDevice, graphicsAndComputeQueueFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(logicalDevice, graphicsAndComputeQueueFamily.value(), 0, &computeQueue);
        vkGetDeviceQueue(logicalDevice, presentationQueueFamily.value(), 0, &presentationQueue);

        queuesInitialized = true;
    }

    bool QueueHandler::isAllQueueFamiliesFound() const {
        return graphicsAndComputeQueueFamily.has_value() && presentationQueueFamily.has_value();
    }

    uint32_t QueueHandler::getGraphicsAndComputeQueueFamily() const {
        assert(queueFamiliesInitialized);
        return graphicsAndComputeQueueFamily.value();
    }

    uint32_t QueueHandler::getPresentationQueueFamily() const {
        assert(queueFamiliesInitialized);
        return presentationQueueFamily.value();
    }

    VkQueue QueueHandler::getGraphicsQueue() const {
        assert(queuesInitialized);
        return graphicsQueue;
    }

    VkQueue QueueHandler::getComputeQueue() const {
        assert(queuesInitialized);
        return computeQueue;
    }

    VkQueue QueueHandler::getPresentationQueue() const {
        assert(queuesInitialized);
        return presentationQueue;
    }

}
