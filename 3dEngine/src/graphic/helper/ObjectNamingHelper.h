#ifndef LASERRIDDLE_OBJECTNAMINGHELPER_H
#define LASERRIDDLE_OBJECTNAMINGHELPER_H

#include <vulkan/vulkan.h>
#include <string>

namespace urchin {

    class ObjectNamingHelper {
        public:
            enum ObjectType {
                PIPELINE,
                RENDER_PASS
            };

            static void nameObject(ObjectType, void*, const std::string&);

        private:
            static std::pair<VkObjectType, std::string> toVkObjectType(ObjectType);
    };

}

#endif
