#pragma once

#include <scene/InputDeviceKey.h>

namespace urchin {

    class Renderer {
        public:
            virtual ~Renderer() = default;

            virtual void onResize(unsigned int, unsigned int) = 0;

            virtual bool onKeyPress(InputDeviceKey) = 0;
            virtual bool onKeyRelease(InputDeviceKey) = 0;
            virtual bool onChar(char32_t) = 0;
            virtual bool onMouseMove(double, double) = 0;
            virtual bool onScroll(double) = 0;
            virtual void onDisable() = 0;

            virtual void prepareRendering(std::uint32_t, float, unsigned int&) = 0;
    };

}
