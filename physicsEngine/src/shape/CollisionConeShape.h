#pragma once

#include <memory>
#include <UrchinCommon.h>

#include <shape/CollisionShape3D.h>
#include <object/CollisionConvexObject3D.h>
#include <utils/math/PhysicsTransform.h>

namespace urchin {

    class CollisionConeShape final : public CollisionShape3D {
        public:
            CollisionConeShape(float, float, ConeShape<float>::ConeOrientation);
            CollisionConeShape(CollisionConeShape&&) noexcept = default;
            CollisionConeShape(const CollisionConeShape&) = delete;
            ~CollisionConeShape() override = default;

            ShapeType getShapeType() const override;
            const ConvexShape3D<float>& getSingleShape() const override;
            float getRadius() const;
            float getHeight() const;
            ConeShape<float>::ConeOrientation getConeOrientation() const;

            std::unique_ptr<CollisionShape3D> scale(const Vector3<float>&) const override;

            AABBox<float> toAABBox(const PhysicsTransform&) const override;
            std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> toConvexObject(const PhysicsTransform&) const override;

            Vector3<float> computeLocalInertia(float) const override;
            float getMaxDistanceToCenter() const override;
            float getMinDistanceToCenter() const override;

            std::unique_ptr<CollisionShape3D> clone() const override;

        private:
            void computeSafeMargin();

            ConeShape<float> coneShape; //shape including margin
    };

}
