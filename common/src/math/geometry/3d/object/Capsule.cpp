#include <iomanip>

#include "math/geometry/3d/object/Capsule.h"

namespace urchin {

    template<class T> Capsule<T>::Capsule():
            ConvexObject3D<T>(ConvexObjectType::CAPSULE),
            capsuleShape(CapsuleShape<T>(0.0, 0.0, CapsuleShape<T>::CAPSULE_X)),
            centerOfMass(Point3<T>(0.0, 0.0, 0.0)) {
        axis[0] = Vector3<T>(0.0, 0.0, 0.0);
        axis[1] = Vector3<T>(0.0, 0.0, 0.0);
        axis[2] = Vector3<T>(0.0, 0.0, 0.0);
    }

    template<class T> Capsule<T>::Capsule(T radius, T cylinderHeight, typename CapsuleShape<T>::CapsuleOrientation capsuleOrientation, const Point3<T>& centerOfMass, const Quaternion<T>& orientation) :
            ConvexObject3D<T>(ConvexObjectType::CAPSULE),
            capsuleShape(CapsuleShape<T>(radius, cylinderHeight, capsuleOrientation)),
            centerOfMass(centerOfMass),
            orientation(orientation) {
        axis[0] = orientation.rotateVector(Vector3<T>(1.0, 0.0, 0.0));
        axis[1] = orientation.rotateVector(Vector3<T>(0.0, 1.0, 0.0));
        axis[2] = orientation.rotateVector(Vector3<T>(0.0, 0.0, 1.0));
    }

    template<class T> T Capsule<T>::getRadius() const {
        return capsuleShape.getRadius();
    }

    template<class T> T Capsule<T>::getCylinderHeight() const {
        return capsuleShape.getCylinderHeight();
    }

    template<class T> typename CapsuleShape<T>::CapsuleOrientation Capsule<T>::getCapsuleOrientation() const {
        return capsuleShape.getCapsuleOrientation();
    }

    template<class T> const Point3<T> &Capsule<T>::getCenterOfMass() const {
        return centerOfMass;
    }

    template<class T> const Quaternion<T> &Capsule<T>::getOrientation() const {
        return orientation;
    }

    /**
     * @return Capsule normalized axis for given index
     */
    template<class T> const Vector3<T> &Capsule<T>::getAxis(unsigned int index) const {
        return axis[index];
    }

    template<class T> Point3<T> Capsule<T>::getSupportPoint(const Vector3<T>& direction) const {
        Vector3<T> normalizedDirection;
        if (direction.X == 0.0 && direction.Y == 0.0 && direction.Z == 0.0) [[unlikely]] {
            normalizedDirection = Vector3<T>(1.0, 0.0, 0.0);
        } else {
            normalizedDirection = direction.normalize();
        }

        Vector3<T> shiftVector = axis[getCapsuleOrientation()] * (T)(getCylinderHeight() / (T)2.0);
        Point3<T> spherePosition1 = centerOfMass.translate(shiftVector);
        Point3<T> spherePosition2 = centerOfMass.translate(-shiftVector);
        Point3<T> supportPoint1 = spherePosition1.translate(normalizedDirection * getRadius());
        Point3<T> supportPoint2 = spherePosition2.translate(normalizedDirection * getRadius());

        if (centerOfMass.vector(supportPoint1).squareLength() > centerOfMass.vector(supportPoint2).squareLength()) {
            return supportPoint1;
        }
        return supportPoint2;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Capsule<T>& capsule) {
        stream.setf(std::ios::left);
        stream << std::setw(20) << "Shape radius: " << capsule.getRadius() << std::endl;
        stream << std::setw(20) << "Shape height: " << capsule.getCylinderHeight() << std::endl;
        stream << std::setw(20) << "Shape orientation: " << capsule.getCapsuleOrientation() << std::endl;
        stream << std::setw(20) << "Center of mass: " << capsule.getCenterOfMass() << std::endl;
        stream << std::setw(20) << "Orientation: " << capsule.getOrientation();

        return stream;
    }

    //explicit template
    template class Capsule<float>;
    template std::ostream& operator <<<float>(std::ostream&, const Capsule<float>&);

    template class Capsule<double>;
    template std::ostream& operator <<<double>(std::ostream&, const Capsule<double>&);

}
