#ifndef URCHINENGINE_CONVEXOBJECT3D_H
#define URCHINENGINE_CONVEXOBJECT3D_H

#include <stdexcept>

#include <math/algebra/point/Point3.h>
#include <math/algebra/vector/Vector3.h>

namespace urchin {
    template<class T> class Point3;
    template<class T> class AABBox;
    template<class T> class Sphere;

    template<class T> class ConvexObject3D {
        public:
            virtual ~ConvexObject3D() = default;

            virtual bool collideWithPoint(const Point3<T>&) const;
            virtual bool collideWithAABBox(const AABBox<T>&) const;
            virtual bool collideWithSphere(const Sphere<T>&) const;

            virtual Point3<T> getSupportPoint(const Vector3<T>&) const = 0;
    };

}

#endif
