#include <sstream>

#include <object/CollisionConeObject.h>

namespace urchin {

    CollisionConeObject::CollisionConeObject(float outerMargin, float radius, float height, typename ConeShape<float>::ConeOrientation coneOrientation,
                                             const Point3<float>& centerOfMass, const Quaternion<float>& orientation) :
            CollisionConvexObject3D(outerMargin),
            coneObject(Cone<float>(radius, height, coneOrientation, centerOfMass, orientation)) {

    }

    /**
     * @return Radius with margin of the cone
     */
    float CollisionConeObject::getRadius() const {
        return coneObject.getRadius() + getOuterMargin();
    }

    float CollisionConeObject::getHeight() const {
        return coneObject.getHeight() + (2.0f * getOuterMargin());
    }

    typename ConeShape<float>::ConeOrientation CollisionConeObject::getConeOrientation() const {
        return coneObject.getConeOrientation();
    }

    const Point3<float>& CollisionConeObject::getCenterOfMass() const {
        return coneObject.getCenterOfMass();
    }

    const Quaternion<float>& CollisionConeObject::getOrientation() const {
        return coneObject.getOrientation();
    }

    /**
     * @return Cone normalized axis for given index
     */
    const Vector3<float>& CollisionConeObject::getAxis(unsigned int index) const {
        return coneObject.getAxis(index);
    }

    CollisionConvexObject3D::ObjectType CollisionConeObject::getObjectType() const {
        return CollisionConvexObject3D::CONE_OBJECT;
    }

    /**
     * @return includeMargin Indicate whether support function need to take into account margin
     */
    Point3<float> CollisionConeObject::getSupportPoint(const Vector3<float>& direction, bool includeMargin) const {
        if (includeMargin) {
            return retrieveCone().getSupportPoint(direction);
        }

        return coneObject.getSupportPoint(direction);
    }

    Cone<float> CollisionConeObject::retrieveCone() const {
        return Cone<float>(getRadius(), getHeight(), getConeOrientation(), getCenterOfMass(), getOrientation());
    }

    std::string CollisionConeObject::toString() const {
        std::stringstream ss;
        ss.precision(std::numeric_limits<float>::max_digits10);

        ss << "Collision cone:" << std::endl;
        ss << std::setw(20) << std::left << " - Outer margin: " << getOuterMargin() << std::endl;
        ss << std::setw(20) << std::left << " - Radius: " << coneObject.getRadius() << std::endl;
        ss << std::setw(20) << std::left << " - Height: " << coneObject.getHeight() << std::endl;
        ss << std::setw(20) << std::left << " - Orientation type: " << getConeOrientation() << std::endl;
        ss << std::setw(20) << std::left << " - Center of mass: " << getCenterOfMass() << std::endl;
        ss << std::setw(20) << std::left << " - Orientation: " << getOrientation();

        return ss.str();
    }

}
