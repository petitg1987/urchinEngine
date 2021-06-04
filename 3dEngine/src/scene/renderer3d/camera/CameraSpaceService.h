#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class CameraSpaceService {
        public:
            explicit CameraSpaceService(Camera*);

            Point2<float> worldSpacePointToScreenSpace(const Point3<float>&) const;
            Ray<float> screenPointToRay(const Point2<float>&, float) const;

        private:
            Camera* camera;
    };

}
