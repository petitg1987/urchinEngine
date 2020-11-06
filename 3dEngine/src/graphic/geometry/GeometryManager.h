#ifndef URCHINENGINE_GEOMETRYMANAGER_H
#define URCHINENGINE_GEOMETRYMANAGER_H

#include <vector>
#include "UrchinCommon.h"

#include "graphic/geometry/GeometryModel.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin {

    class GeometryManager {
        public:
            virtual ~GeometryManager() = default;

            void addGeometry(GeometryModel *);
            void removeGeometry(GeometryModel *);

            void onCameraProjectionUpdate(const Camera *);

            void display(const Matrix4<float> &viewMatrix) const;

        private:
            std::vector<GeometryModel *> geometryModels;
            Matrix4<float> projectionMatrix;
    };

}

#endif
