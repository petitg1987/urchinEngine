#include <cassert>

#include <math/geometry/3d/shape/ConeShape.h>
#include <math/geometry/3d/object/Cone.h>
#include <math/algorithm/MathFunction.h>

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
        #ifdef URCHIN_DEBUG //TODO review
            assert(MathFunction::isEqual((T)transform.getScale().X, (T)transform.getScale().Z, (T)0.001));
        #endif
        return std::make_unique<Cone<T>>(radius * transform.getScale().X, height * transform.getScale().Y,
                coneOrientation, transform.getPosition(), transform.getOrientation());
    }

    //explicit template
    template class ConeShape<float>;
    template class ConeShape<double>;

}
