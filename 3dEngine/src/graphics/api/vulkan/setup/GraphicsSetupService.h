#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "graphics/setup/FramebufferSizeRetriever.h"
#include "graphics/setup/SurfaceCreator.h"
#include "graphics/api/vulkan/setup/handler/ValidationLayer.h"
#include "graphics/api/vulkan/setup/handler/DeviceHandler.h"
#include "graphics/api/vulkan/setup/handler/QueueHandler.h"
#include "graphics/api/vulkan/render/handler/SwapChainHandler.h"

VK_DEFINE_HANDLE(VmaAllocator)

namespace urchin {

    class GraphicsSetupService {
        public:
            static GraphicsSetupService& instance();
            ~GraphicsSetupService();

            void initialize(const std::vector<std::string>&, const std::unique_ptr<SurfaceCreator>&, FramebufferSizeRetriever&);
            void cleanup();

            const FramebufferSizeRetriever* getFramebufferSizeRetriever() const;
            VkSurfaceKHR getSurface() const;
            const DeviceHandler& getDevices() const;
            const QueueHandler& getQueues() const;
            VkCommandPool getAllocateCommandPool() const;
            VmaAllocator getAllocator() const;
            const ValidationLayer& getValidationLayer() const;

        private:
            GraphicsSetupService();

            void createInstance(const std::vector<std::string>&);

            void createAllocateCommandPool();
            void createAllocator();

            FramebufferSizeRetriever* framebufferSizeRetriever;
            bool isInitialized;
            const uint32_t vulkanVersion;
            VkInstance vkInstance;
            VkSurfaceKHR surface;
            std::unique_ptr<ValidationLayer> validationLayer;
            std::unique_ptr<DeviceHandler> deviceHandler;
            QueueHandler queueHandler;
            VkCommandPool allocateCommandPool;
            VmaAllocator allocator;
    };

}
