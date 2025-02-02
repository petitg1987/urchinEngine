#include <iomanip>

#include <math/geometry/3d/object/Cylinder.h>

namespace urchin {

    template<class T> Cylinder<T>::Cylinder():
            ConvexObject3D<T>(ConvexObjectType::CYLINDER),
            cylinderShape(CylinderShape<T>(0.0, 0.0, CylinderShape<T>::CYLINDER_X)),
            centerOfMass(Point3<T>(0.0, 0.0, 0.0)) {
        axis[0] = Vector3<T>(0.0, 0.0, 0.0);
        axis[1] = Vector3<T>(0.0, 0.0, 0.0);
        axis[2] = Vector3<T>(0.0, 0.0, 0.0);
    }

    template<class T> Cylinder<T>::Cylinder(T radius, T height, typename CylinderShape<T>::CylinderOrientation cylinderOrientation, const Point3<T>& centerOfMass, const Quaternion<T>& orientation) :
            ConvexObject3D<T>(ConvexObjectType::CYLINDER),
            cylinderShape(CylinderShape<T>(radius, height, cylinderOrientation)),
            centerOfMass(centerOfMass),
            orientation(orientation) {
        axis[0] = orientation.rotateVector(Vector3<T>(1.0, 0.0, 0.0));
        axis[1] = orientation.rotateVector(Vector3<T>(0.0, 1.0, 0.0));
        axis[2] = orientation.rotateVector(Vector3<T>(0.0, 0.0, 1.0));
    }

    template<class T> T Cylinder<T>::getRadius() const {
        return cylinderShape.getRadius();
    }

    template<class T> T Cylinder<T>::getHeight() const {
        return cylinderShape.getHeight();
    }

    template<class T> typename CylinderShape<T>::CylinderOrientation Cylinder<T>::getCylinderOrientation() const {
        return cylinderShape.getCylinderOrientation();
    }

    template<class T> const Point3<T> &Cylinder<T>::getCenterOfMass() const {
        return centerOfMass;
    }

    template<class T> const Quaternion<T> &Cylinder<T>::getOrientation() const {
        return orientation;
    }

    /**
     * @return Cylinder normalized axis for given index
     */
    template<class T> const Vector3<T> &Cylinder<T>::getAxis(unsigned int index) const {
        return axis[index];
    }

    template<class T> Point3<T> Cylinder<T>::getSupportPoint(const Vector3<T>& direction) const {
        Vector3<T> normalizedDirection;
        if (direction.X == 0.0 && direction.Y == 0.0 && direction.Z == 0.0) [[unlikely]] {
            normalizedDirection = Vector3<T>(1.0, 0.0, 0.0);
        } else {
            normalizedDirection = direction.normalize();
        }
        Vector3<T> projectedDirectionOnCircle = normalizedDirection - (normalizedDirection.dotProduct(axis[getCylinderOrientation()]) * axis[getCylinderOrientation()]);
        if (projectedDirectionOnCircle.squareLength() > 0.0) {
            projectedDirectionOnCircle = projectedDirectionOnCircle.normalize();
        }

        Vector3<T> shiftVector = axis[getCylinderOrientation()] * (T)(getHeight() / (T)2.0);
        Point3<T> circlePosition1 = centerOfMass.translate(shiftVector);
        Point3<T> circlePosition2 = centerOfMass.translate(-shiftVector);
        Point3<T> supportPoint1 = circlePosition1.translate(projectedDirectionOnCircle * getRadius());
        Point3<T> supportPoint2 = circlePosition2.translate(projectedDirectionOnCircle * getRadius());

        if (normalizedDirection.dotProduct(centerOfMass.vector(supportPoint1).normalize()) > normalizedDirection.dotProduct(centerOfMass.vector(supportPoint2).normalize())) {
            return supportPoint1;
        }
        return supportPoint2;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Cylinder<T>& cylinder) {
        stream.setf(std::ios::left);
        stream << std::setw(20) << "Shape radius: " << cylinder.getRadius() << std::endl;
        stream << std::setw(20) << "Shape height: " << cylinder.getHeight() << std::endl;
        stream << std::setw(20) << "Shape orientation: " << cylinder.getCylinderOrientation() << std::endl;
        stream << std::setw(20) << "Center of mass: " << cylinder.getCenterOfMass() << std::endl;
        stream << std::setw(20) << "Orientation: " << cylinder.getOrientation();

        return stream;
    }

    //explicit template
    template class Cylinder<float>;
    template std::ostream& operator <<<float>(std::ostream&, const Cylinder<float>&);

    template class Cylinder<double>;
    template std::ostream& operator <<<double>(std::ostream&, const Cylinder<double>&);

}
