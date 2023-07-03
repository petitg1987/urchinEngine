#pragma once

#include <stdexcept>

#include <math/algebra/point/Point3.h>
#include <math/algebra/vector/Vector3.h>

namespace urchin {
    template<class T> class Point3;
    template<class T> class AABBox;
    template<class T> class OBBox;
    template<class T> class Sphere;
    template<class T> class Ray;
    template<class T> class Line3D;

    enum class ConvexObjectType {
            AABBOX,
            CAPSULE,
            CONE,
            CONVEX_HULL,
            CYLINDER,
            FRUSTUM,
            LINE_SEGMENT,
            OBBOX,
            SPHERE,
            TETRAHEDRON,
            TRIANGLE
    };

    template<class T> class ConvexObject3D {
        public:
            explicit ConvexObject3D(ConvexObjectType);
            virtual ~ConvexObject3D() = default;

            ConvexObjectType getObjectType() const;

            virtual bool collideWithPoint(const Point3<T>&) const;
            virtual bool collideWithAABBox(const AABBox<T>&) const;
            virtual bool collideWithOBBox(const OBBox<T>&) const;
            virtual bool collideWithSphere(const Sphere<T>&) const;
            virtual bool collideWithRay(const Ray<T>&) const;
            virtual Point3<T> intersectPoint(const Line3D<T>&, bool&) const;

            virtual Point3<T> getSupportPoint(const Vector3<T>&) const = 0;

        private:
            ConvexObjectType objectType;
    };

}
