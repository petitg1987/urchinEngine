#ifndef URCHINENGINE_SURFACECREATOR_H
#define URCHINENGINE_SURFACECREATOR_H

#include <vulkan/vulkan.h>

namespace urchin {

    class SurfaceCreator {
        public:
            virtual VkSurfaceKHR createSurface(VkInstance) const = 0;
    };

}

#endif
