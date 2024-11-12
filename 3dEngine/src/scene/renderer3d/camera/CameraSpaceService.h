#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class CameraSpaceService {
        public:
            explicit CameraSpaceService(const Camera&);

            Point2<float> worldSpacePointToNdcSpace(const Point3<float>&) const;
            Point2<float> worldSpacePointToScreenSpace(const Point3<float>&) const;

            Ray<float> screenPointToRay(const Point2<float>&, float) const;
            Line3D<float> screenPointToLine(const Point2<float>&) const;

        private:
            Vector3<float> screenPointToDirection(const Point2<float>&) const;

            const Camera& camera;
    };

}
