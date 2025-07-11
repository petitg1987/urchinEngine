#pragma once

#include <memory>
#include <array>

#include <math/geometry/3d/shape/ConvexShape3D.h>
#include <math/algebra/point/Point3.h>

namespace urchin {

    template<class T> class TriangleShape3D final : public ConvexShape3D<T> { //TODO merge in triangle3d ?
        public:
            explicit TriangleShape3D(const std::array<Point3<T>, 3>&);
            TriangleShape3D(const Point3<T>&, const Point3<T>&, const Point3<T>&);

            const std::array<Point3<T>, 3>& getPoints() const;

            std::unique_ptr<ConvexShape3D<T>> clone() const override;
            std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T>&) const override;

        private:
            std::array<Point3<T>, 3> points;
    };

}
