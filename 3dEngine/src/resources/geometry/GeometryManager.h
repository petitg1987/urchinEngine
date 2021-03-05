#ifndef URCHINENGINE_GEOMETRYMANAGER_H
#define URCHINENGINE_GEOMETRYMANAGER_H

#include <vector>
#include "UrchinCommon.h"

#include "graphic/render/target/RenderTarget.h"
#include "resources/geometry/GeometryModel.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin {

    class GeometryManager {
        public:
            explicit GeometryManager(std::shared_ptr<RenderTarget>);
            virtual ~GeometryManager() = default;

            void addGeometry(GeometryModel*);
            void removeGeometry(GeometryModel*);

            void onCameraProjectionUpdate(const Camera*);

            void display(const Matrix4<float>&) const;

        private:
            std::shared_ptr<RenderTarget> renderTarget;

            std::vector<GeometryModel*> geometryModels;
            Matrix4<float> projectionMatrix;
    };

}

#endif
