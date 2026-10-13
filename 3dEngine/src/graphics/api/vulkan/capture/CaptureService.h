#pragma once

#include <string>
#include <vulkan/vulkan.h>

#include "graphics/api/vulkan/texture/Texture.h"

namespace urchin {

    class CaptureService {
        public:
            static CaptureService& instance();

            void takeCapture(const std::string&, const Texture&, unsigned int = 0, unsigned int = 0) const;

        private:
            CaptureService() = default;

            void cmdPipelineBarrier(VkImage, VkCommandBuffer, VkPipelineStageFlags2, VkAccessFlags2, VkPipelineStageFlags2, VkAccessFlags2, VkImageLayout, VkImageLayout) const;
    };

}
