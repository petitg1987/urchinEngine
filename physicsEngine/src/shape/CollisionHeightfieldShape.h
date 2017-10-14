#ifndef URCHINENGINE_COLLISIONHEIGHTFIELDSHAPE_H
#define URCHINENGINE_COLLISIONHEIGHTFIELDSHAPE_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"

namespace urchin
{

    class CollisionHeightfieldShape : public CollisionShape3D
    {
        public:
            CollisionHeightfieldShape(std::vector<Point3<float>>, unsigned int, unsigned int);

            CollisionShape3D::ShapeType getShapeType() const override;
            std::shared_ptr<ConvexShape3D<float>> getSingleShape() const override;

            std::shared_ptr<CollisionShape3D> scale(float) const override;

            AABBox<float> toAABBox(const PhysicsTransform &) const override;
            std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const override;

            Vector3<float> computeLocalInertia(float) const override;
            float getMaxDistanceToCenter() const override;
            float getMinDistanceToCenter() const override;

        private:
            std::unique_ptr<BoxShape<float>> buildLocalAABBox() const;

            std::vector<Point3<float>> vertices;
            unsigned int xLength;
            unsigned int zLength;

            std::unique_ptr<BoxShape<float>> localAABBox;

            mutable AABBox<float> lastAABBox;
            mutable PhysicsTransform lastTransform;
    };

}

#endif
