#ifndef URCHINENGINE_COMMANDBUFFERHELPER_H
#define URCHINENGINE_COMMANDBUFFERHELPER_H

#include <vulkan/vulkan.h>

namespace urchin {

    class CommandBufferHelper {
        public:
            static VkCommandBuffer beginSingleTimeCommands();
            static void endSingleTimeCommands(VkCommandBuffer);
    };

}

#endif
