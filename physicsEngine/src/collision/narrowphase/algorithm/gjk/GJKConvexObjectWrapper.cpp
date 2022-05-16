#include <collision/narrowphase/algorithm/gjk/GJKConvexObjectWrapper.h>

namespace urchin {

    GJKConvexObjectWrapper::GJKConvexObjectWrapper(const CollisionConvexObject3D& collisionConvexObject, bool includeMargin) :
            collisionConvexObject(collisionConvexObject),
            includeMargin(includeMargin) {

    }

    Point3<float> GJKConvexObjectWrapper::getSupportPoint(const Vector3<float>& direction) const {
        return collisionConvexObject.getSupportPoint(direction, includeMargin);
    }

    std::string GJKConvexObjectWrapper::toString() const {
        return  collisionConvexObject.toString() + "\n - Include margin: " + std::to_string(includeMargin);
    }

}
