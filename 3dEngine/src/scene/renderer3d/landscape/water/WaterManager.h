#ifndef URCHINENGINE_WATERMANAGER_H
#define URCHINENGINE_WATERMANAGER_H

#include "UrchinCommon.h"

#include "graphic/render/target/TargetRenderer.h"
#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/landscape/water/Water.h"
#include "scene/renderer3d/landscape/fog/FogManager.h"

namespace urchin {

    class WaterManager {
        public:
            explicit WaterManager(const TargetRenderer *);

            void onCameraProjectionUpdate(const Camera *);

            void addWater(Water *);
            void removeWater(Water *);

            void display(const Camera *, FogManager *, float) const;

        private:
            const TargetRenderer *renderTarget;

            std::vector<Water *> waters;

            Matrix4<float> projectionMatrix;
    };

}

#endif
