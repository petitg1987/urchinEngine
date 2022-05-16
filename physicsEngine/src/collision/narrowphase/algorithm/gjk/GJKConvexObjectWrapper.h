#pragma once

#include <object/CollisionConvexObject3D.h>

namespace urchin {

    class GJKConvexObjectWrapper {
        public:
            GJKConvexObjectWrapper(const CollisionConvexObject3D&, bool);

            Point3<float> getSupportPoint(const Vector3<float>&) const;

            std::string toString() const;

        private:
            const CollisionConvexObject3D& collisionConvexObject;
            bool includeMargin;
    };

}