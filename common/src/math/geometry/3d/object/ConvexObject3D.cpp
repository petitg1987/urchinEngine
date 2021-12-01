#include <math/geometry/3d/object/ConvexObject3D.h>

namespace urchin {

    template<class T> ConvexObject3D<T>::ConvexObject3D(ConvexObjectType objectType) :
            objectType(objectType) {

    }

    template<class T> ConvexObjectType ConvexObject3D<T>::getObjectType() const {
        return objectType;
    }

    template<class T> bool ConvexObject3D<T>::collideWithPoint(const Point3<T>&) const {
        throw std::runtime_error("Collision test unsupported.");
    }

    /**
    * @return True if the bounding box collides or is inside this convex object
    */
    template<class T> bool ConvexObject3D<T>::collideWithAABBox(const AABBox<T>&) const {
        throw std::runtime_error("Collision test unsupported.");
    }

    /**
    * @return True if the sphere collides or is inside this convex object
    */
    template<class T> bool ConvexObject3D<T>::collideWithSphere(const Sphere<T>&) const {
        throw std::runtime_error("Collision test unsupported.");
    }

    //explicit template
    template class ConvexObject3D<float>;
    template class ConvexObject3D<double>;

}
