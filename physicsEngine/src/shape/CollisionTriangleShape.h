#ifndef URCHINENGINE_COLLISIONTRIANGLESHAPE_H
#define URCHINENGINE_COLLISIONTRIANGLESHAPE_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "object/CollisionConvexObject3D.h"
#include "object/CollisionTriangleObject.h"
#include "utils/math/PhysicsTransform.h"
#include "utils/pool/FixedSizePool.h"

namespace urchin
{

    class CollisionTriangleShape : public CollisionShape3D
    {
        public:
            explicit CollisionTriangleShape(const Point3<float> *);
            explicit CollisionTriangleShape(const std::shared_ptr<TriangleShape3D<float>> &);
            CollisionTriangleShape(CollisionTriangleShape &&) noexcept;
            CollisionTriangleShape(const CollisionTriangleShape &) = delete;
            ~CollisionTriangleShape() override;

            CollisionShape3D::ShapeType getShapeType() const override;
            std::shared_ptr<ConvexShape3D<float>> getSingleShape() const override;

            std::shared_ptr<CollisionShape3D> scale(float) const override;

            AABBox<float> toAABBox(const PhysicsTransform &) const override;
            void setupConvexObjectPool(FixedSizePool<CollisionTriangleObject> *);
            CollisionConvexObject3D *toConvexObject(const PhysicsTransform &) const override;

            Vector3<float> computeLocalInertia(float) const override;
            float getMaxDistanceToCenter() const override;
            float getMinDistanceToCenter() const override;

            CollisionShape3D *clone() const override;

        private:
            std::shared_ptr<TriangleShape3D<float>> triangleShape; //shape including margin

            mutable CollisionTriangleObject *lastConvexObject;
            FixedSizePool<CollisionTriangleObject> *collisionTriangleObjectsPool;

    };

}

#endif
