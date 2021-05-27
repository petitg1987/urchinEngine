#ifndef URCHINENGINE_DEBUGLABELHELPER_H
#define URCHINENGINE_DEBUGLABELHELPER_H

#include <vulkan/vulkan.h>
#include <string>
#include "UrchinCommon.h"

namespace urchin {

    class DebugLabelHelper {
        public:
            enum ObjectType {
                PIPELINE,
                RENDER_PASS,
                SHADER
            };

            static void nameObject(ObjectType, void*, const std::string&);

            static void beginDebugRegion(VkCommandBuffer, const std::string& label, const Vector4<float>&);
            static void endDebugRegion(VkCommandBuffer);

        private:
            static std::pair<VkObjectType, std::string> toVkObjectType(ObjectType);
    };

}

#endif
