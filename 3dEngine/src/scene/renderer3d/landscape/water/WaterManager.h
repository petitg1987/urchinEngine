#pragma once

#include <UrchinCommon.h>

#include <graphic/render/target/RenderTarget.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/landscape/water/Water.h>
#include <scene/renderer3d/landscape/fog/FogManager.h>

namespace urchin {

    class WaterManager {
        public:
            explicit WaterManager(RenderTarget&);

            void onCameraProjectionUpdate(const Camera&);

            void addWater(const std::shared_ptr<Water>&);
            void removeWater(const Water&);

            void prepareRendering(const Camera&, FogManager*, float) const;

        private:
            RenderTarget& renderTarget;

            std::vector<std::shared_ptr<Water>> waters;

            Matrix4<float> projectionMatrix;
    };

}
