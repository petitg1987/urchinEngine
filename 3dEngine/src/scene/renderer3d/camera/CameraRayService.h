#ifndef URCHINENGINE_CAMERARAYSERVICE_H
#define URCHINENGINE_CAMERARAYSERVICE_H

#include "UrchinCommon.h"

#include "scene/renderer3d/camera/Camera.h"

namespace urchin
{

    class CameraRayService
    {
        public:
            CameraRayService(Camera *);

            Ray<float> screenPointToRay(const Point2<float> &, float) const;

        private:
            Camera *camera;
    };

}

#endif
