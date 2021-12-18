#include <cassert>

#include <math/geometry/3d/shape/CylinderShape.h>
#include <math/geometry/3d/object/Cylinder.h>
#include <math/algorithm/MathFunction.h>
#include <util/StringUtil.h>

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
        if (!MathFunction::isEqual((T)transform.getScale().X, (T)transform.getScale().Z, (T)0.001)) {
            throw std::runtime_error("Cylinder cannot by transformed with a different scale on X and Z axis: " + StringUtil::toString(transform.getScale()));
        }

        return std::make_unique<Cylinder<T>>(radius * transform.getScale().X, height * transform.getScale().Y,
                cylinderOrientation, transform.getPosition(), transform.getOrientation());
    }

    //explicit template
    template class CylinderShape<float>;
    template class CylinderShape<double>;

}
