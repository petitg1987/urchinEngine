#include <cmath>
#include <iomanip>

#include <math/geometry/3d/Line3D.h>
#include <math/geometry/3d/object/AABBox.h>
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

    template<class T> const Point3<T>& Sphere<T>::getCenterOfMass() const {
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

    /**
     * @return True if the box collides or is inside this sphere
     */
    template<class T> bool Sphere<T>::collideWithAABBox(const AABBox<T>& bbox) const {
        T squareDistance = 0.0;
        for (std::size_t i = 0; i < 3; i++) {
            T centerAxis = centerOfMass[i];
            if (centerAxis < bbox.getMin()[i]) {
                squareDistance += (bbox.getMin()[i] - centerAxis) * (bbox.getMin()[i] - centerAxis);
            }
            if (centerAxis > bbox.getMax()[i]) {
                squareDistance += (centerAxis - bbox.getMax()[i]) * (centerAxis - bbox.getMax()[i]);
            }
        }
        return squareDistance <= getRadius() * getRadius();
    }

    template<class T> Point3<T> Sphere<T>::intersectPoint(const Line3D<T>& line, bool& hasIntersection) const {
        Vector3<T> ab = line.getA().vector(line.getB()).normalize();
        Vector3<T> aToCenter = line.getA().vector(centerOfMass);
        T squareRadius = getRadius() * getRadius();

        const T t = aToCenter.dotProduct(ab);
        Point3<T> tPoint = line.getA().translate(ab * t);

        T squareLengthCenterToTPoint = centerOfMass.vector(tPoint).squareLength();
        if (squareLengthCenterToTPoint > squareRadius) {
            hasIntersection = false;
            return Point3<T>(0.0f, 0.0f, 0.0f);
        }

        hasIntersection = true;
        T tToT0Distance = std::sqrt(squareRadius - squareLengthCenterToTPoint);
        T t0 = t - tToT0Distance;
        return line.getA().translate(ab * t0);
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Sphere<T>& sphere) {
        stream.setf(std::ios::left);
        stream << std::setw(20) << "Shape radius: " << sphere.getRadius() << std::endl;
        stream << std::setw(20) << "Center of mass: " << sphere.getCenterOfMass() << std::endl;

        return stream;
    }

    //explicit template
    template class Sphere<float>;
    template std::ostream& operator <<<float>(std::ostream&, const Sphere<float>&);

    template class Sphere<double>;
    template std::ostream& operator <<<double>(std::ostream&, const Sphere<double>&);

}
