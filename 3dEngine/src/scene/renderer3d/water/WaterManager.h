#ifndef URCHINENGINE_WATERMANAGER_H
#define URCHINENGINE_WATERMANAGER_H

#include "UrchinCommon.h"

#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/water/Water.h"

namespace urchin
{

    class WaterManager
    {
        public:
            void onCameraProjectionUpdate(const Camera *);

            void addWater(Water *);
            void removeWater(Water *);

            void display(const Camera *, float) const;

        private:
            std::vector<Water *> waters;

            Matrix4<float> projectionMatrix;
    };

}

#endif
