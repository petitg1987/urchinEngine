#pragma once

#include <UrchinCommon.h>

#include <graphics/api/vulkan/render/target/ScreenRender.h>

namespace urchin {

    class CaptureService : public Singleton<CaptureService> {
        public:
            friend class Singleton<CaptureService>;

            void takeCapture(const std::string&, VkImage, VkFormat, VkImageLayout, unsigned int, unsigned int, unsigned int = 0, unsigned int = 0) const;

        private:
            CaptureService() = default;

            void cmdPipelineBarrier(VkImage, VkCommandBuffer, VkAccessFlags, VkAccessFlags, VkImageLayout, VkImageLayout) const;
    };

}
