#pragma once

#include <vulkan/vulkan.h>

namespace urchin {

    struct CommandBufferData {
        VkCommandBuffer commandBuffer;
        std::string_view name;
    };

    class CommandBufferHelper {
        public:
            static CommandBufferData beginSingleTimeCommands(std::string_view);
            static void endSingleTimeCommands(const CommandBufferData&);
    };

}
