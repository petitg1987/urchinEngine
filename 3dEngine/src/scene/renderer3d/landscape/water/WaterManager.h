#ifndef URCHINENGINE_WATERMANAGER_H
#define URCHINENGINE_WATERMANAGER_H

#include "UrchinCommon.h"

#include "graphic/render/target/RenderTarget.h"
#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/landscape/water/Water.h"
#include "scene/renderer3d/landscape/fog/FogManager.h"

namespace urchin {

    class WaterManager {
        public:
            explicit WaterManager(const RenderTarget *);

            void onCameraProjectionUpdate(const Camera *);

            void addWater(Water *);
            void removeWater(Water *);

            void display(const Camera *, FogManager *, float) const;

        private:
            const RenderTarget *renderTarget;

            std::vector<Water *> waters;

            Matrix4<float> projectionMatrix;
    };

}

#endif
