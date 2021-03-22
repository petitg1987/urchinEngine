#ifndef URCHINENGINE_QUEUEHANDLER_H
#define URCHINENGINE_QUEUEHANDLER_H

#include <vulkan/vulkan.h>
#include <optional>

namespace urchin {

    class QueueHandler {
        public:
            QueueHandler();

            void initializeQueueFamilies(VkPhysicalDevice, VkSurfaceKHR);
            void initializeQueues(VkDevice);

            bool isAllQueueFamiliesFound() const;
            uint32_t getGraphicsQueueFamily() const;
            uint32_t getPresentationQueueFamily() const;

            VkQueue getGraphicsQueue() const;
            VkQueue getPresentationQueue() const;

        private:
            static bool isGraphicsQueueFamily(VkQueueFamilyProperties);
            static bool isPresentationQueueFamily(uint32_t, VkPhysicalDevice, VkSurfaceKHR);

            bool queueFamiliesInitialized;
            std::optional<uint32_t> graphicsQueueFamily;
            std::optional<uint32_t> presentationQueueFamily;

            bool queuesInitialized;
            VkQueue graphicsQueue;
            VkQueue presentationQueue;


    };

}

#endif
