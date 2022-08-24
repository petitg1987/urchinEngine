#pragma once

#include <string>
#include <UrchinCommon.h>

#include <object/CollisionConvexObject3D.h>

namespace urchin {

    class CollisionCapsuleObject final : public CollisionConvexObject3D {
        public:
            CollisionCapsuleObject(float, float, float, typename CapsuleShape<float>::CapsuleOrientation, const Point3<float>&, const Quaternion<float>&);

            float getRadius() const;
            float getCylinderHeight() const;
            typename CapsuleShape<float>::CapsuleOrientation getCapsuleOrientation() const;
            const Point3<float>& getCenterOfMass() const;
            const Quaternion<float>& getOrientation() const;
            const Vector3<float>& getAxis(unsigned int) const;

            CollisionConvexObject3D::ObjectType getObjectType() const override;
            Point3<float> getSupportPoint(const Vector3<float>&, bool) const override;

            Capsule<float> retrieveCapsule() const;
            std::string toString() const override;

        private:
            const Capsule<float> capsuleObject; //object without margin
    };

}
