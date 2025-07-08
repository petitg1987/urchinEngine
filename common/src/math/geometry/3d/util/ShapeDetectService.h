#pragma once

#include <memory>
#include <vector>

#include <math/algebra/point/Point3.h>
#include <math/algebra/Quaternion.h>
#include <math/geometry/3d/shape/ConvexShape3D.h>
#include <math/geometry/3d/shape/BoxShape.h>
#include <math/geometry/3d/shape/SphereShape.h>

namespace urchin {

    class ShapeDetectService {
        public:
            struct LocalizedShape {
                std::unique_ptr<ConvexShape3D<float>> shape;
                Point3<float> position;
                Quaternion<float> orientation;
            };

            std::vector<LocalizedShape> detect(const std::vector<Point3<float>>&) const;

        private:
            std::unique_ptr<BoxShape<float>> tryBuildBox(const std::vector<Point3<float>>&, Point3<float>&, Quaternion<float>&) const;
            std::unique_ptr<SphereShape<float>> tryBuildSphere(const std::vector<Point3<float>>&, Point3<float>&, Quaternion<float>&) const;

            std::pair<std::size_t, std::size_t> findClosestAndFarthestPoints(const std::vector<Point3<float>>&, const Point3<float>&) const;
            std::size_t findFarthestPoint(const std::vector<Point3<float>>&, const Point3<float>&) const;
    };

}
