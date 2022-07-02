#pragma once

namespace urchin {

    class SurfaceCreator {
        public:
            virtual ~SurfaceCreator() = default;

            virtual void* createSurface(void*) = 0;
    };

}
