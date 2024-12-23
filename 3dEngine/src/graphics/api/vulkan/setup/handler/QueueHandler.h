#pragma once

#include <vulkan/vulkan.h>
#include <optional>

namespace urchin {

    class QueueHandler {
        public:
            QueueHandler();
            QueueHandler(const QueueHandler&) = delete;

            void initializeQueueFamilies(VkPhysicalDevice, VkSurfaceKHR);
            void initializeQueues(VkDevice);

            bool isAllQueueFamiliesFound() const;
            uint32_t getGraphicsAndComputeQueueFamily() const;
            uint32_t getPresentationQueueFamily() const;

            VkQueue getGraphicsAndComputeQueue() const;
            VkQueue getPresentationQueue() const;

        private:
            bool isGraphicsAndComputeQueueFamily(const VkQueueFamilyProperties&) const;
            bool isPresentationQueueFamily(uint32_t, VkPhysicalDevice, VkSurfaceKHR) const;

            bool queueFamiliesInitialized;
            std::optional<uint32_t> graphicsAndComputeQueueFamily;
            std::optional<uint32_t> presentationQueueFamily;

            bool queuesInitialized;
            VkQueue graphicsAndComputeQueue;
            VkQueue presentationQueue;


    };

}
