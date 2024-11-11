#pragma once

#include <vulkan/vulkan.h>

namespace urchin {

    class CaptureService {
        public:
            static CaptureService& instance();

            void takeCapture(const std::string&, VkImage, VkFormat, unsigned int, unsigned int, unsigned int = 0, unsigned int = 0) const;

        private:
            CaptureService() = default;

            void cmdPipelineBarrier(VkImage, VkCommandBuffer, VkAccessFlags, VkAccessFlags, VkImageLayout, VkImageLayout) const;
    };

}
