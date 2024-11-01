#pragma once

#include <UrchinCommon.h>
#include <memory>
#include <vulkan/vulkan.h>

#include <graphics/setup/FramebufferSizeRetriever.h>
#include <graphics/setup/SurfaceCreator.h>
#include <graphics/api/vulkan/setup/handler/ValidationLayer.h>
#include <graphics/api/vulkan/setup/handler/DeviceHandler.h>
#include <graphics/api/vulkan/setup/handler/QueueHandler.h>
#include <graphics/api/vulkan/render/handler/SwapChainHandler.h>

VK_DEFINE_HANDLE(VmaAllocator)

namespace urchin {

    class GraphicsSetupService final : public Singleton<GraphicsSetupService> {
        public:
            friend class Singleton;

            ~GraphicsSetupService() override;

            static void enableUniqueSurface();
            static void destroySurface();

            bool isInitialized() const;
            void initialize(const std::vector<std::string>&, std::unique_ptr<SurfaceCreator>, FramebufferSizeRetriever&);

            const FramebufferSizeRetriever* getFramebufferSizeRetriever() const;
            static VkSurfaceKHR getSurface();
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

            static bool useUniqueSurface;
            static VkInstance vkInstance;
            static VkSurfaceKHR surface;

            FramebufferSizeRetriever* framebufferSizeRetriever;
            bool apiGraphicInitialized;
            const uint32_t vulkanVersion;
            ValidationLayer validationLayer;
            DeviceHandler deviceHandler;
            QueueHandler queueHandler;
            VkCommandPool allocateCommandPool;
            VmaAllocator allocator;
    };

}
