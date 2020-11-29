#ifndef URCHINENGINE_CYLINDER_H
#define URCHINENGINE_CYLINDER_H

#include "math/geometry/3d/object/ConvexObject3D.h"
#include "math/geometry/3d/shape/CylinderShape.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/Quaternion.h"

namespace urchin {

    template<class T> class Cylinder : public ConvexObject3D<T> {
        public:
            Cylinder();
            Cylinder(T, T, typename CylinderShape<T>::CylinderOrientation, const Point3<T>&, const Quaternion<T>&);

            T getRadius() const;
            T getHeight() const;
            typename CylinderShape<T>::CylinderOrientation getCylinderOrientation() const;
            const Point3<T> &getCenterOfMass() const;
            const Quaternion<T> &getOrientation() const;
            const Vector3<T> &getAxis(unsigned int) const;

            Point3<T> getSupportPoint(const Vector3<T>&) const;

        private:
            CylinderShape<T> cylinderShape;
            Point3<T> centerOfMass;
            Quaternion<T> orientation;

            Vector3<T> axis[3]; //3 vectors of normalized axis
    };

}

 #endif
