#include <cassert>

#include <math/geometry/3d/shape/CapsuleShape.h>
#include <math/geometry/3d/object/Capsule.h>

namespace urchin {

    template<class T> CapsuleShape<T>::CapsuleShape(T radius, T cylinderHeight, CapsuleOrientation capsuleOrientation) :
            radius(radius),
            cylinderHeight(cylinderHeight),
            capsuleOrientation(capsuleOrientation) {
        assert(radius >= 0.0f);
        assert(cylinderHeight >= 0.0f);
    }

    template<class T> T CapsuleShape<T>::getRadius() const {
        return radius;
    }

    template<class T> T CapsuleShape<T>::getCylinderHeight() const {
        return cylinderHeight;
    }

    template<class T> typename CapsuleShape<T>::CapsuleOrientation CapsuleShape<T>::getCapsuleOrientation() const {
        return capsuleOrientation;
    }

    template<class T> T CapsuleShape<T>::computeHeight() const {
        return cylinderHeight + (T)2.0 * radius;
    }

    template<class T> std::unique_ptr<ConvexShape3D<T>> CapsuleShape<T>::clone() const {
        return std::make_unique<CapsuleShape<T>>(*this);
    }

    template<class T> std::unique_ptr<ConvexObject3D<T>> CapsuleShape<T>::toConvexObject(const Transform<T>& transform) const {
        return std::make_unique<Capsule<T>>(radius * transform.getScale(), cylinderHeight * transform.getScale(),
                capsuleOrientation, transform.getPosition(), transform.getOrientation());
    }

    //explicit template
    template class CapsuleShape<float>;
    template class CapsuleShape<double>;
}
