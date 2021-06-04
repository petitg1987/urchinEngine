#pragma once

#include <string>
#include <UrchinCommon.h>

namespace urchin {

    class CollisionConvexObject3D {
        public:
            enum ObjectType {
                SPHERE_OBJECT = 0,
                BOX_OBJECT,
                CAPSULE_OBJECT,
                CYLINDER_OBJECT,
                CONE_OBJECT,
                CONVEX_HULL_OBJECT,
                TRIANGLE_OBJECT
            };

            explicit CollisionConvexObject3D(float);
            virtual ~CollisionConvexObject3D() = default;

            float getOuterMargin() const;

            virtual CollisionConvexObject3D::ObjectType getObjectType() const = 0;
            virtual Point3<float> getSupportPoint(const Vector3<float>&, bool) const = 0;

            virtual std::string toString() const = 0;

        private:
            const float outerMargin;
    };

}
