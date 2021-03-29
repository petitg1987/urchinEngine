#ifndef URCHINENGINE_GRAPHICSERVICE_H
#define URCHINENGINE_GRAPHICSERVICE_H

#include "UrchinCommon.h"
#include <memory>
#include <vulkan/vulkan.h>

#include "graphic/setup/spi/SurfaceCreator.h"
#include "graphic/setup/spi/FramebufferSizeRetriever.h"
#include "graphic/setup/handler/ValidationLayer.h"
#include "graphic/setup/handler/DeviceHandler.h"
#include "graphic/setup/handler/QueueHandler.h"
#include "graphic/render/handler/SwapChainHandler.h"

VK_DEFINE_HANDLE(VmaAllocator)

namespace urchin {

    class GraphicService : public Singleton<GraphicService> {
        public:
            friend class Singleton<GraphicService>;

            void initialize(const std::vector<const char*>&, const std::unique_ptr<SurfaceCreator>&, std::unique_ptr<FramebufferSizeRetriever>);

            const std::unique_ptr<FramebufferSizeRetriever>& getFramebufferSizeRetriever() const;
            VkSurfaceKHR getSurface() const;
            DeviceHandler getDevices() const;
            QueueHandler getQueues() const;
            VkCommandPool getAllocateCommandPool() const;
            VmaAllocator getAllocator() const;

            const ValidationLayer& getValidationLayer() const;

        private:
            GraphicService();
            ~GraphicService() override;

            void createInstance(const std::vector<const char*>&);
            void createAllocateCommandPool();
            void createAllocator();

            std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever;
            bool apiGraphicInitialized;

            const uint32_t vulkanVersion;
            VkInstance vkInstance;
            VkSurfaceKHR surface;
            ValidationLayer validationLayer;
            DeviceHandler deviceHandler;
            QueueHandler queueHandler;
            VkCommandPool allocateCommandPool;
            VmaAllocator allocator;
    };

}

#endif
