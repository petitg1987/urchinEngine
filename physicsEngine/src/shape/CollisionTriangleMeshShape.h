#ifndef URCHINENGINE_COLLISIONTRIANGLEMESHSHAPE_H
#define URCHINENGINE_COLLISIONTRIANGLEMESHSHAPE_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"

namespace urchin
{

    class CollisionTriangleMeshShape : public CollisionShape3D
    {
        public:
            CollisionTriangleMeshShape();

            CollisionShape3D::ShapeType getShapeType() const override;
            std::shared_ptr<ConvexShape3D<float>> getSingleShape() const override;

            std::shared_ptr<CollisionShape3D> scale(float) const override;

            AABBox<float> toAABBox(const PhysicsTransform &) const override;
            std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const override;

            Vector3<float> computeLocalInertia(float) const override;
            float getMaxDistanceToCenter() const override;
            float getMinDistanceToCenter() const override;

        private:
            mutable AABBox<float> lastAabbox;
            mutable PhysicsTransform lastTransform;
    };

}

#endif
