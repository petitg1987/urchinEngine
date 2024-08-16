#pragma once

#include <cmath>
#include <array>

#include <math/geometry/3d/object/ConvexObject3D.h>
#include <math/geometry/3d/shape/ConeShape.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/vector/Vector3.h>
#include <math/algebra/Quaternion.h>

namespace urchin {

    template<class T> class Cone final : public ConvexObject3D<T> {
        public:
            Cone();
            Cone(T, T, typename ConeShape<T>::ConeOrientation, const Point3<T>&, const Quaternion<T>&);

            T getRadius() const;
            T getHeight() const;
            typename ConeShape<T>::ConeOrientation getConeOrientation() const;
            const Point3<T>& getCenterOfMass() const;
            const Quaternion<T>& getOrientation() const;
            const Vector3<T>& getAxis(unsigned int) const;

            Point3<T> getCenter() const;
            Point3<T> getSupportPoint(const Vector3<T>&) const override;

        private:
            ConeShape<T> coneShape;
            Point3<T> centerOfMass;
            Quaternion<T> orientation;

            std::array<Vector3<T>, 3> axis; //3 vectors of normalized axis
            T baseSideCosAngle;
    };

    template<class T> std::ostream& operator <<(std::ostream&, const Cone<T>&);

}
