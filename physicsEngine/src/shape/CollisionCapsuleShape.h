#pragma once

#include <memory>
#include <UrchinCommon.h>

#include <shape/CollisionShape3D.h>
#include <object/CollisionConvexObject3D.h>
#include <utils/math/PhysicsTransform.h>

namespace urchin {

    class CollisionCapsuleShape final : public CollisionShape3D {
        public:
            CollisionCapsuleShape(float, float, CapsuleShape<float>::CapsuleOrientation);
            CollisionCapsuleShape(CollisionCapsuleShape&&) noexcept = default;
            CollisionCapsuleShape(const CollisionCapsuleShape&) = delete;
            ~CollisionCapsuleShape() override = default;

            ShapeType getShapeType() const override;
            const ConvexShape3D<float>& getSingleShape() const override;
            float getRadius() const;
            float getCylinderHeight() const;
            CapsuleShape<float>::CapsuleOrientation getCapsuleOrientation() const;

            std::unique_ptr<CollisionShape3D> scale(const Vector3<float>&) const override;

            AABBox<float> toAABBox(const PhysicsTransform&) const override;
            std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> toConvexObject(const PhysicsTransform&) const override;

            Vector3<float> computeLocalInertia(float) const override;
            float getMaxDistanceToCenter() const override;
            float getMinDistanceToCenter() const override;

            std::unique_ptr<CollisionShape3D> clone() const override;

        private:
            void computeSafeMargin();

            CapsuleShape<float> capsuleShape; //shape including margin
    };

}
