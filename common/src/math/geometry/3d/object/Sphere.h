#pragma once

#include <math/geometry/3d/object/ConvexObject3D.h>
#include <math/geometry/3d/shape/SphereShape.h>
#include <math/algebra/point/Point3.h>

namespace urchin {

    template<class T> class Sphere : public ConvexObject3D<T> {
        public:
            Sphere();
            Sphere(T, const Point3<T>&);

            T getRadius() const;
            const Point3<T>& getCenterOfMass() const;

            Point3<T> getSupportPoint(const Vector3<T>&) const override;

            bool collideWithPoint(const Point3<T>&) const override;
            bool collideWithSphere(const Sphere<T>&) const override;
            bool collideWithAABBox(const AABBox<T>&) const override;
            Point3<T> intersectPoint(const Line3D<T>&, bool&) const override;

        private:
            SphereShape<T> sphereShape;
            Point3<T> centerOfMass;
    };

}
