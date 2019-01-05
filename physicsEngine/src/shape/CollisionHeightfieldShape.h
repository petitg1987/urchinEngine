#ifndef URCHINENGINE_COLLISIONHEIGHTFIELDSHAPE_H
#define URCHINENGINE_COLLISIONHEIGHTFIELDSHAPE_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "shape/CollisionConcaveShape.h"
#include "object/CollisionTriangleObject.h"
#include "utils/pool/FixedSizePool.h"

namespace urchin
{

    class CollisionHeightfieldShape : public CollisionShape3D, public CollisionConcaveShape
    {
        public:
            CollisionHeightfieldShape(std::vector<Point3<float>>, unsigned int, unsigned int);
            CollisionHeightfieldShape(CollisionHeightfieldShape &&) = delete;
            CollisionHeightfieldShape(const CollisionHeightfieldShape &) = delete;
            ~CollisionHeightfieldShape() override;

            CollisionShape3D::ShapeType getShapeType() const override;
            const ConvexShape3D<float> *getSingleShape() const override;
            const std::vector<Point3<float>> &getVertices() const;
            unsigned int getXLength() const;
            unsigned int getZLength() const;

            std::shared_ptr<CollisionShape3D> scale(float) const override;

            AABBox<float> toAABBox(const PhysicsTransform &) const override;
            CollisionConvexObject3D *toConvexObject(const PhysicsTransform &) const override;

            Vector3<float> computeLocalInertia(float) const override;
            float getMaxDistanceToCenter() const override;
            float getMinDistanceToCenter() const override;

            CollisionShape3D *clone() const override;

            const std::vector<CollisionTriangleShape> &findTrianglesInAABBox(const AABBox<float> &) const override;
            const std::vector<CollisionTriangleShape> &findTrianglesHitByRay(const LineSegment3D<float> &) const override;

        private:
            enum Axis{X, Z};

            std::unique_ptr<BoxShape<float>> buildLocalAABBox() const;
            std::pair<unsigned int, unsigned int> computeStartEndIndices(float, float, Axis) const;
            void createTrianglesMatchHeight(unsigned int, unsigned int, float, float) const;
            void createCollisionTriangleShape(const Point3<float> &, const Point3<float> &, const Point3<float> &) const;

            std::vector<Point3<float>> vertices;
            unsigned int xLength;
            unsigned int zLength;

            std::unique_ptr<BoxShape<float>> localAABBox;

            mutable std::vector<CollisionTriangleShape> trianglesInAABBox;
            FixedSizePool<TriangleShape3D<float>> *triangleShapesPool;
            FixedSizePool<CollisionTriangleObject> *collisionTriangleObjectsPool;
    };

}

#endif
