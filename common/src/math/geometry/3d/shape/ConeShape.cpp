#include <cassert>

#include <math/geometry/3d/shape/ConeShape.h>
#include <math/geometry/3d/object/Cone.h>
#include <math/algorithm/MathFunction.h>
#include <util/StringUtil.h>

namespace urchin {

    template<class T> ConeShape<T>::ConeShape(T radius, T height, ConeOrientation coneOrientation) :
            radius(radius),
            height(height),
            coneOrientation(coneOrientation) {
        assert(radius >= 0.0f);
        assert(height >= 0.0f);
    }

    template<class T> T ConeShape<T>::getRadius() const {
        return radius;
    }

    template<class T> T ConeShape<T>::getHeight() const {
        return height;
    }

    template<class T> typename ConeShape<T>::ConeOrientation ConeShape<T>::getConeOrientation() const {
        return coneOrientation;
    }

    template<class T> std::unique_ptr<ConvexShape3D<T>> ConeShape<T>::clone() const {
        return std::make_unique<ConeShape<T>>(*this);
    }

    template<class T> std::unique_ptr<ConvexObject3D<T>> ConeShape<T>::toConvexObject(const Transform<T>& transform) const {
        std::size_t heightAxis = 0;
        if (coneOrientation == ConeOrientation::CONE_X_POSITIVE || coneOrientation == ConeOrientation::CONE_X_NEGATIVE) {
            heightAxis = 0;
        } else if (coneOrientation == ConeOrientation::CONE_Y_POSITIVE || coneOrientation == ConeOrientation::CONE_Y_NEGATIVE) {
            heightAxis = 1;
        } else if (coneOrientation == ConeOrientation::CONE_Z_POSITIVE || coneOrientation == ConeOrientation::CONE_Z_NEGATIVE) {
            heightAxis = 2;
        }

        if (!MathFunction::isEqual((T)transform.getScale()[(heightAxis + 1) % 3], (T)transform.getScale()[(heightAxis + 2) % 3], (T)0.01)) {
            throw std::runtime_error("Cone cannot by transformed with scale: " + StringUtil::toString(transform.getScale()));
        }

        T radiusScale = transform.getScale()[(heightAxis + 1) % 3];
        T heightScale = transform.getScale()[heightAxis];
        return std::make_unique<Cone<T>>(radius * radiusScale, height * heightScale, coneOrientation, transform.getPosition(), transform.getOrientation());
    }

    //explicit template
    template class ConeShape<float>;
    template class ConeShape<double>;

}
