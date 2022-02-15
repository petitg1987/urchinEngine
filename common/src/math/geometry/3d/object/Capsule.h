#pragma once

#include <iostream>

#include <math/geometry/3d/object/ConvexObject3D.h>
#include <math/geometry/3d/shape/CapsuleShape.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/vector/Vector3.h>
#include <math/algebra/Quaternion.h>

namespace urchin {

    template<class T> class Capsule : public ConvexObject3D<T> {
        public:
            Capsule();
            Capsule(T, T, typename CapsuleShape<T>::CapsuleOrientation, const Point3<T>&, const Quaternion<T>&);

            T getRadius() const;
            T getCylinderHeight() const;
            typename CapsuleShape<T>::CapsuleOrientation getCapsuleOrientation() const;
            const Point3<T>& getCenterOfMass() const;
            const Quaternion<T>& getOrientation() const;
            const Vector3<T>& getAxis(unsigned int) const;

            Point3<T> getSupportPoint(const Vector3<T>&) const override;

        private:
            CapsuleShape<T> capsuleShape;
            Point3<T> centerOfMass;
            Quaternion<T> orientation;

            std::array<Vector3<T>, 3> axis; //3 vectors of normalized axis
    };

    template<class T> std::ostream& operator <<(std::ostream&, const Capsule<T>&);

}
