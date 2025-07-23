#include <cassert>

#include "object/CollisionTriangleObject.h"

namespace urchin {
    CollisionTriangleObject::CollisionTriangleObject(float outerMargin, const Point3<float>& point1, const Point3<float>& point2, const Point3<float>& point3) :
            CollisionConvexObject3D(outerMargin),
            triangleObject(Triangle3D(point1, point2, point3)) {

    }

    const Point3<float>& CollisionTriangleObject::getPoint(unsigned int index) const {
        return triangleObject.getPoints()[index];
    }

    CollisionConvexObject3D::ObjectType CollisionTriangleObject::getObjectType() const {
        return TRIANGLE_OBJECT;
    }

    Point3<float> CollisionTriangleObject::getSupportPoint(const Vector3<float>& direction, bool includeMargin) const {
        if (includeMargin) {
            return retrieveTriangle().getSupportPoint(direction);
        }

        return triangleObject.getSupportPoint(direction);
    }

    Triangle3D<float> CollisionTriangleObject::retrieveTriangle() const {
        assert(getOuterMargin() == 0.0f);

        return triangleObject;
    }

    std::string CollisionTriangleObject::toString() const {
        std::stringstream ss;
        ss.precision(std::numeric_limits<float>::max_digits10);

        ss << "Collision triangle:" << std::endl;
        ss << std::setw(20) << std::left << " - Outer margin: " << getOuterMargin() << std::endl;
        ss << std::setw(20) << std::left << " - Point 0: " << triangleObject.getPoints()[0] << std::endl;
        ss << std::setw(20) << std::left << " - Point 1: " << triangleObject.getPoints()[1] << std::endl;
        ss << std::setw(20) << std::left << " - Point 2: " << triangleObject.getPoints()[2] << std::endl;

        return ss.str();
    }
}
