#include <math/geometry/3d/object/Sphere.h>

namespace urchin {

    template<class T> Sphere<T>::Sphere() :
            ConvexObject3D<T>(ConvexObjectType::SPHERE),
            sphereShape(SphereShape<T>(0.0)),
            centerOfMass(Point3<T>(0.0, 0.0, 0.0)) {

    }

    template<class T> Sphere<T>::Sphere(T radius, const Point3<T>& centerOfMass) :
            ConvexObject3D<T>(ConvexObjectType::SPHERE),
            sphereShape(SphereShape<T>(radius)),
            centerOfMass(centerOfMass) {

    }

    template<class T> T Sphere<T>::getRadius() const {
        return sphereShape.getRadius();
    }

    template<class T> const Point3<T> &Sphere<T>::getCenterOfMass() const {
        return centerOfMass;
    }

    template<class T> Point3<T> Sphere<T>::getSupportPoint(const Vector3<T>& direction) const {
        if (direction.X == 0.0 && direction.Y == 0.0 && direction.Z == 0.0) [[unlikely]] {
            return centerOfMass + Point3<T>(getRadius(), (T)0.0, (T)0.0);
        }

        return centerOfMass.translate(direction.normalize() * getRadius());
    }

    template<class T> bool Sphere<T>::collideWithPoint(const Point3<T>& point) const {
        return centerOfMass.squareDistance(point) <= this->getRadius() * this->getRadius();
    }

    /**
    * @return True if the sphere collides or is inside this sphere
    */
    template<class T> bool Sphere<T>::collideWithSphere(const Sphere<T>& sphere) const {
        T sumRadius = this->getRadius() + sphere.getRadius();
        return centerOfMass.squareDistance(sphere.getCenterOfMass()) <= (sumRadius * sumRadius);
    }

    //explicit template
    template class Sphere<float>;
    template class Sphere<double>;

}
