#pragma once

#include <UrchinCommon.h>
#include <memory>
#include <vulkan/vulkan.h>

#include <graphic/setup/spi/SurfaceCreator.h>
#include <graphic/setup/spi/FramebufferSizeRetriever.h>
#include <graphic/setup/handler/ValidationLayer.h>
#include <graphic/setup/handler/DeviceHandler.h>
#include <graphic/setup/handler/QueueHandler.h>
#include <graphic/render/handler/SwapChainHandler.h>

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
            DeviceHandler getDevices() const;
            QueueHandler getQueues() const;
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
