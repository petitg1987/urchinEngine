#pragma once

#include <memory>
#include <vector>

#include <math/algebra/point/Point3.h>
#include <math/algebra/Quaternion.h>
#include <math/geometry/3d/shape/ConvexShape3D.h>

namespace urchin {

    class ShapeDetectService {
        public:
            std::unique_ptr<ConvexShape3D<float>> detect(const std::vector<Point3<float>>&, Point3<float>&, Quaternion<float>&) const;
    };

}
