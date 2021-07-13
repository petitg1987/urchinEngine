#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <graphic/render/target/RenderTarget.h>
#include <resources/geometry/GeometryModel.h>
#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class GeometryManager {
        public:
            explicit GeometryManager(RenderTarget&);

            void addGeometry(GeometryModel*);
            void removeGeometry(GeometryModel*);

            void onCameraProjectionUpdate(const Camera&);

            void prepareRendering(const Matrix4<float>&) const;

        private:
            RenderTarget& renderTarget;

            std::vector<GeometryModel*> geometryModels;
            Matrix4<float> projectionMatrix;
    };

}
