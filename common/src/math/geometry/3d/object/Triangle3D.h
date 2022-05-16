#pragma once

#include <math/geometry/3d/object/ConvexObject3D.h>
#include <math/geometry/3d/shape/TriangleShape3D.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/vector/Vector3.h>

namespace urchin {

    template<class T> class Triangle3D : public ConvexObject3D<T> {
        public:
            explicit Triangle3D(const std::array<Point3<T>, 3>&);
            Triangle3D(const Point3<T>&, const Point3<T>&, const Point3<T>&);

            const std::array<Point3<T>, 3>& getPoints() const;

            Point3<T> getSupportPoint(const Vector3<T>&) const override;
            Vector3<T> computeNormal() const;
            Point3<T> closestPoint(const Point3<T>&, std::array<T, 3>&) const;
            bool projectedPointInsideTriangle(const Point3<T>&) const;

        private:
            TriangleShape3D<T> triangleShape;
    };

    template<class T> std::ostream& operator <<(std::ostream&, const Triangle3D<T>&);

}
