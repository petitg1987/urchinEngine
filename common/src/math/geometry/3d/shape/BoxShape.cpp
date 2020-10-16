#include <algorithm>
#include <cassert>

#include "BoxShape.h"
#include "math/geometry/3d/object/OBBox.h"

namespace urchin {

    template<class T> BoxShape<T>::BoxShape() :
            halfSizes(Vector3<T>(0.0, 0.0, 0.0)) {

    }

    /**
     * @param halfSizes Half sizes (width, height and depth) of box
     */
    template<class T> BoxShape<T>::BoxShape(const Vector3<T> &halfSizes) :
            halfSizes(halfSizes) {
        assert(halfSizes.X >= 0.0);
        assert(halfSizes.Y >= 0.0);
        assert(halfSizes.Z >= 0.0);
    }

    template<class T> T BoxShape<T>::getHalfSize(unsigned int index) const {
        return halfSizes[index];
    }

    template<class T> const Vector3<T> &BoxShape<T>::getHalfSizes() const {
        return halfSizes;
    }

    template<class T> T BoxShape<T>::getMaxHalfSize() const {
        return std::max(std::max(halfSizes[0], halfSizes[1]), halfSizes[2]);
    }

    template<class T> unsigned int BoxShape<T>::getMaxHalfSizeIndex() const {
        if (halfSizes[0] > halfSizes[1]) {
            if (halfSizes[0] > halfSizes[2]) {
                return 0;
            }
            return 1;
        } else if (halfSizes[1] > halfSizes[2]) {
            return 1;
        }
        return 2;
    }

    template<class T> T BoxShape<T>::getMinHalfSize() const {
        return std::min(std::min(halfSizes[0], halfSizes[1]), halfSizes[2]);
    }

    template<class T> unsigned int BoxShape<T>::getMinHalfSizeIndex() const {
        if (halfSizes[0] < halfSizes[1]) {
            if (halfSizes[0] < halfSizes[2]) {
                return 0;
            }
            return 1;
        } else if (halfSizes[1] < halfSizes[2]) {
            return 1;
        }
        return 2;
    }

    template<class T> Point3<T> BoxShape<T>::getSupportPoint(const Vector3<T> &direction) const {
        return Point3<T>(
                direction.X < 0.0 ? -halfSizes[0] : halfSizes[0],
                direction.Y < 0.0 ? -halfSizes[1] : halfSizes[1],
                direction.Z < 0.0 ? -halfSizes[2] : halfSizes[2]);
    }

    template<class T> T BoxShape<T>::getVolume() const {
        return halfSizes.X * halfSizes.Y * halfSizes.Z * 8.0;
    }

    template<class T> ConvexShape3D<T> *BoxShape<T>::clone() const {
        return new BoxShape<T>(*this);
    }

    template<class T> std::unique_ptr<ConvexObject3D<T>> BoxShape<T>::toConvexObject(const Transform<T> &transform) const {
        return std::make_unique<OBBox<T>>(halfSizes*transform.getScale(), transform.getPosition(), transform.getOrientation());
    }

    //explicit template
    template class BoxShape<float>;
    template class BoxShape<double>;
}
