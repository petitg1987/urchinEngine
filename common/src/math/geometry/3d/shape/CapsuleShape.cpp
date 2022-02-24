#include <cassert>

#include <math/geometry/3d/shape/CapsuleShape.h>
#include <math/geometry/3d/object/Capsule.h>
#include <math/algorithm/MathFunction.h>
#include <util/StringUtil.h>

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
        std::size_t heightAxis = 0;
        if (capsuleOrientation == CapsuleOrientation::CAPSULE_X) {
            heightAxis = 0;
        } else if (capsuleOrientation == CapsuleOrientation::CAPSULE_Y) {
            heightAxis = 1;
        } else if (capsuleOrientation == CapsuleOrientation::CAPSULE_Z) {
            heightAxis = 2;
        }

        if (!MathFunction::isEqual((T)transform.getScale()[(heightAxis + 1) % 3], (T)transform.getScale()[(heightAxis + 2) % 3], (T)0.01)) {
            throw std::runtime_error("Capsule cannot by transformed with scale: " + StringUtil::toString(transform.getScale()));
        }

        T radiusScale = transform.getScale()[(heightAxis + 1) % 3];
        T heightScale = transform.getScale()[heightAxis];
        T originalCylinderHeight = (radius * (T)2.0) + cylinderHeight;
        T newHeight = std::max((T)0.001, (originalCylinderHeight * heightScale) - 2.0f * (radius * radiusScale));
        return std::make_unique<Capsule<T>>(radius * radiusScale, newHeight, capsuleOrientation, transform.getPosition(), transform.getOrientation());
    }

    //explicit template
    template class CapsuleShape<float>;
    template class CapsuleShape<double>;
}
