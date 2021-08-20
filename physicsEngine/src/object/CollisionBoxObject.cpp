#include <sstream>

#include <object/CollisionBoxObject.h>

namespace urchin {

    CollisionBoxObject::CollisionBoxObject(float outerMargin, const Vector3<float>& halfSize, const Point3<float>& centerOfMass, const Quaternion<float>& orientation) :
            CollisionConvexObject3D(outerMargin),
            boxObject(OBBox<float>(halfSize, centerOfMass, orientation)) {

    }

    float CollisionBoxObject::getHalfSize(unsigned int index) const {
        return boxObject.getHalfSize(index) + getOuterMargin();
    }

    Vector3<float> CollisionBoxObject::getHalfSizes() const {
        return boxObject.getHalfSizes() + Vector3<float>(getOuterMargin(), getOuterMargin(), getOuterMargin());
    }

    const Point3<float>& CollisionBoxObject::getCenterOfMass() const {
        return boxObject.getCenterOfMass();
    }

    const Quaternion<float>& CollisionBoxObject::getOrientation() const {
        return boxObject.getOrientation();
    }

    const Vector3<float>& CollisionBoxObject::getNormalizedAxis(unsigned int index) const {
        return boxObject.getNormalizedAxis(index);
    }

    CollisionConvexObject3D::ObjectType CollisionBoxObject::getObjectType() const {
        return CollisionConvexObject3D::BOX_OBJECT;
    }

    /**
     * @return includeMargin Indicate whether support function need to take into account margin
     */
    Point3<float> CollisionBoxObject::getSupportPoint(const Vector3<float>& direction, bool includeMargin) const {
        if (includeMargin) {
            const Point3<float>& supportPoint = boxObject.getSupportPoint(direction);
            Point3<float> supportPointWithMargin = supportPoint;

            for (unsigned int i = 0; i < 3; ++i) { //for each axis
                const Vector3<float>& axis = boxObject.getNormalizedAxis(i);
                if (axis.dotProduct(boxObject.getCenterOfMass().vector(supportPoint)) > 0.0f) {
                    supportPointWithMargin = supportPointWithMargin.translate(axis * getOuterMargin());
                } else {
                    supportPointWithMargin = supportPointWithMargin.translate(-(axis * getOuterMargin()));
                }
            }

            return supportPointWithMargin;
        }

        return boxObject.getSupportPoint(direction);
    }

    OBBox<float> CollisionBoxObject::retrieveOBBox() const {
        return OBBox<float>(getHalfSizes(), getCenterOfMass(), getOrientation());
    }

    std::string CollisionBoxObject::toString() const {
        std::stringstream ss;
        ss.precision(std::numeric_limits<float>::max_digits10);

        ss << "Collision box:" << std::endl;
        ss << std::setw(20) << std::left << " - Outer margin: " << getOuterMargin() << std::endl;
        ss << std::setw(20) << std::left << " - Half size: " << boxObject.getHalfSizes() << std::endl;
        ss << std::setw(20) << std::left << " - Center of mass: " << getCenterOfMass() << std::endl;
        ss << std::setw(20) << std::left << " - Orientation: " << getOrientation();

        return ss.str();
    }

}
