#include <stdexcept>

#include <math/geometry/3d/object/ConvexObject3D.h>

namespace urchin {

    template<class T> ConvexObject3D<T>::ConvexObject3D(ConvexObjectType objectType) :
            objectType(objectType) {

    }

    template<class T> ConvexObjectType ConvexObject3D<T>::getObjectType() const {
        return objectType;
    }

    template<class T> bool ConvexObject3D<T>::collideWithPoint(const Point3<T>&) const {
        throw std::runtime_error("Collide with point: unsupported");
    }

    template<class T> bool ConvexObject3D<T>::collideWithAABBox(const AABBox<T>&) const {
        throw std::runtime_error("Collide with AABBox: unsupported");
    }

    template<class T> bool ConvexObject3D<T>::collideWithOBBox(const OBBox<T>&) const {
        throw std::runtime_error("Collide with OBBox: unsupported");
    }

    template<class T> bool ConvexObject3D<T>::collideWithSphere(const Sphere<T>&) const {
        throw std::runtime_error("Collide with sphere: unsupported");
    }

    template<class T>  bool ConvexObject3D<T>::collideWithRay(const Ray<T>&) const {
        throw std::runtime_error("Collide with ray: unsupported");
    }

    template<class T> Point3<T> ConvexObject3D<T>::intersectPoint(const Line3D<T>&, bool&) const {
        throw std::runtime_error("Intersection test unsupported.");
    }

    //explicit template
    template class ConvexObject3D<float>;
    template class ConvexObject3D<double>;

}
