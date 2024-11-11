#include <collision/narrowphase/algorithm/gjk/GJKConvexObjectWrapper.h>

namespace urchin {

    GJKConvexObjectWrapper::GJKConvexObjectWrapper(const CollisionConvexObject3D& collisionConvexObject, bool includeMargin) :
            collisionConvexObject(collisionConvexObject),
            includeMargin(includeMargin) {

    }

    const CollisionConvexObject3D& GJKConvexObjectWrapper::getCollisionConvexObject() const {
        return collisionConvexObject;
    }

    bool GJKConvexObjectWrapper::isIncludeMargin() const {
        return includeMargin;
    }

    Point3<float> GJKConvexObjectWrapper::getSupportPoint(const Vector3<float>& direction) const {
        return collisionConvexObject.getSupportPoint(direction, includeMargin);
    }

    Point3<double> GJKConvexObjectWrapper::getSupportPoint(const Vector3<double>& direction) const {
        return collisionConvexObject.getSupportPoint(direction.cast<float>(), includeMargin).cast<double>();
    }

    std::ostream& operator <<(std::ostream& stream, const GJKConvexObjectWrapper& collisionConvexObject) {
        stream.setf(std::ios::left);
        stream << std::setw(20) << collisionConvexObject.getCollisionConvexObject().toString() << std::endl;
        stream << std::setw(20) << " - Include margin: " << collisionConvexObject.isIncludeMargin() << std::endl;

        return stream;
    }

}
