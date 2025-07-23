#include <cassert>

#include "math/geometry/3d/shape/CylinderShape.h"
#include "math/geometry/3d/object/Cylinder.h"
#include "math/algorithm/MathFunction.h"
#include "util/StringUtil.h"

namespace urchin {

    template<class T> CylinderShape<T>::CylinderShape(T radius, T height, CylinderOrientation cylinderOrientation) :
            radius(radius),
            height(height),
            cylinderOrientation(cylinderOrientation) {
        assert(radius >= 0.0f);
        assert(height >= 0.0f);
    }

    template<class T> T CylinderShape<T>::getRadius() const {
        return radius;
    }

    template<class T> T CylinderShape<T>::getHeight() const {
        return height;
    }

    template<class T> typename CylinderShape<T>::CylinderOrientation CylinderShape<T>::getCylinderOrientation() const {
        return cylinderOrientation;
    }

    template<class T> std::unique_ptr<ConvexShape3D<T>> CylinderShape<T>::clone() const {
        return std::make_unique<CylinderShape<T>>(*this);
    }

    template<class T> std::unique_ptr<ConvexObject3D<T>> CylinderShape<T>::toConvexObject(const Transform<T>& transform) const {
        std::size_t heightAxis = 0;
        if (cylinderOrientation == CylinderOrientation::CYLINDER_X) {
            heightAxis = 0;
        } else if (cylinderOrientation == CylinderOrientation::CYLINDER_Y) {
            heightAxis = 1;
        } else if (cylinderOrientation == CylinderOrientation::CYLINDER_Z) {
            heightAxis = 2;
        }

        if (!MathFunction::isEqual((T)transform.getScale()[(heightAxis + 1) % 3], (T)transform.getScale()[(heightAxis + 2) % 3], (T)0.01)) {
            throw std::runtime_error("Cylinder cannot by transformed with scale: " + StringUtil::toString(transform.getScale()));
        }

        T radiusScale = transform.getScale()[(heightAxis + 1) % 3];
        T heightScale = transform.getScale()[heightAxis];
        return std::make_unique<Cylinder<T>>(radius * radiusScale, height * heightScale, cylinderOrientation, transform.getPosition(), transform.getOrientation());
    }

    //explicit template
    template class CylinderShape<float>;
    template class CylinderShape<double>;

}
