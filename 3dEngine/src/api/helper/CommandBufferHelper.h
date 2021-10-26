#pragma once

#include <vulkan/vulkan.h>

namespace urchin {

    class CommandBufferHelper {
        public:
            static VkCommandBuffer beginSingleTimeCommands();
            static void endSingleTimeCommands(VkCommandBuffer);
    };

}
