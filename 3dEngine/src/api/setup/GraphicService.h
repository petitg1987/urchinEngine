#pragma once

#include <UrchinCommon.h>
#include <memory>
#include <vulkan/vulkan.h>

#include <api/setup/spi/SurfaceCreator.h>
#include <api/setup/spi/FramebufferSizeRetriever.h>
#include <api/setup/handler/ValidationLayer.h>
#include <api/setup/handler/DeviceHandler.h>
#include <api/setup/handler/QueueHandler.h>
#include <api/render/handler/SwapChainHandler.h>

VK_DEFINE_HANDLE(VmaAllocator)

namespace urchin {

    class GraphicService : public Singleton<GraphicService> {
        public:
            friend class Singleton<GraphicService>;

            ~GraphicService() override;

            static void enableUniqueSurface();
            static void destroySurface();

            void initialize(const std::vector<std::string>&, std::unique_ptr<SurfaceCreator>, FramebufferSizeRetriever&);

            const FramebufferSizeRetriever* getFramebufferSizeRetriever() const;
            static VkSurfaceKHR getSurface();
            const DeviceHandler& getDevices() const;
            const QueueHandler& getQueues() const;
            VkCommandPool getAllocateCommandPool() const;
            VmaAllocator getAllocator() const;

            const ValidationLayer& getValidationLayer() const;

        private:
            GraphicService();

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
