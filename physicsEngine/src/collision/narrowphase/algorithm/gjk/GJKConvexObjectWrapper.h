#pragma once

#include <object/CollisionConvexObject3D.h>

namespace urchin {

    class GJKConvexObjectWrapper {
        public:
            GJKConvexObjectWrapper(const CollisionConvexObject3D&, bool);

            const CollisionConvexObject3D& getCollisionConvexObject() const;
            bool isIncludeMargin() const;

            Point3<float> getSupportPoint(const Vector3<float>&) const;
            Point3<double> getSupportPoint(const Vector3<double>&) const;

        private:
            const CollisionConvexObject3D& collisionConvexObject;
            bool includeMargin;
    };

    std::ostream& operator <<(std::ostream&, const GJKConvexObjectWrapper&);

}