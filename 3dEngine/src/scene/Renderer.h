#pragma once

namespace urchin {

    class Renderer {
        public:
            virtual ~Renderer() = default;

            virtual void onResize(unsigned int, unsigned int) = 0;

            virtual bool onKeyPress(unsigned int) = 0;
            virtual bool onKeyRelease(unsigned int) = 0;
            virtual bool onChar(unsigned int) = 0;
            virtual bool onMouseMove(int, int) = 0;
            virtual void onDisable() = 0;

            virtual void prepareRendering(float) = 0;
    };

}
