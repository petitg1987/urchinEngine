#ifndef URCHINENGINE_COLLISIONSHERESHAPE_H
#define URCHINENGINE_COLLISIONSHERESHAPE_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "object/CollisionConvexObject3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin {

    class CollisionSphereShape : public CollisionShape3D {
        public:
            explicit CollisionSphereShape(float);
            CollisionSphereShape(CollisionSphereShape&&) noexcept;
            CollisionSphereShape(const CollisionSphereShape&) = delete;
            ~CollisionSphereShape() override;

            CollisionShape3D::ShapeType getShapeType() const override;
            const ConvexShape3D<float>* getSingleShape() const override;
            float getRadius() const;

            std::shared_ptr<CollisionShape3D> scale(float) const override;

            AABBox<float> toAABBox(const PhysicsTransform&) const override;
            std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> toConvexObject(const PhysicsTransform&) const override;

            Vector3<float> computeLocalInertia(float) const override;
            float getMaxDistanceToCenter() const override;
            float getMinDistanceToCenter() const override;

            CollisionShape3D* clone() const override;

        private:
            SphereShape<float>* sphereShape; //shape including margin
    };

}

#endif
