#pragma once

#include <vulkan/vulkan.h>

namespace urchin {

    class SurfaceCreator {
        public:
            virtual ~SurfaceCreator() = default;

            virtual VkSurfaceKHR createSurface(VkInstance) = 0;
    };

}
