#include <limits>
#include <cmath>

#include "CollisionTriangleMeshShape.h"

namespace urchin
{

    CollisionTriangleMeshShape::CollisionTriangleMeshShape() :
            CollisionShape3D()
    {
        lastTransform.setPosition(Point3<float>(NAN, NAN, NAN));
    }

    CollisionShape3D::ShapeType CollisionTriangleMeshShape::getShapeType() const
    {
        return CollisionTriangleMeshShape::TRIANGLE_MESH_SHAPE;
    }

    std::shared_ptr<ConvexShape3D<float>> CollisionTriangleMeshShape::getSingleShape() const
    {
        throw std::runtime_error("Impossible to retrieve single convex shape for triangle mesh shape");
    }

    std::shared_ptr<CollisionShape3D> CollisionTriangleMeshShape::scale(float scale) const
    {
        if((scale - std::numeric_limits<float>::min()) > 1.0 || (scale + std::numeric_limits<float>::min()) < 0.0)
        {
            throw std::runtime_error("Scaling a triangle mesh shape is currently not supported");
        }

        return std::make_shared<CollisionTriangleMeshShape>();
    }

    AABBox<float> CollisionTriangleMeshShape::toAABBox(const PhysicsTransform &physicsTransform) const
    {
        if(!lastTransform.equals(physicsTransform))
        {
            //TODO complete

            lastTransform = physicsTransform;
        }

        return lastAabbox;
    }

    std::shared_ptr<CollisionConvexObject3D> CollisionTriangleMeshShape::toConvexObject(const PhysicsTransform &physicsTransform) const
    {
        throw std::runtime_error("Impossible to transform triangle mesh shape to convex object");
    }

    Vector3<float> CollisionTriangleMeshShape::computeLocalInertia(float mass) const
    {
        AABBox<float> aabbox = toAABBox(PhysicsTransform());
        float width = 2.0f * aabbox.getHalfSize(0);
        float height = 2.0f * aabbox.getHalfSize(1);
        float depth = 2.0f * aabbox.getHalfSize(2);

        float localInertia1 = (1.0f/12.0f) * mass * (height*height + depth*depth);
        float localInertia2 = (1.0f/12.0f) * mass * (width*width + depth*depth);
        float localInertia3 = (1.0f/12.0f) * mass * (width*width + height*height);
        return Vector3<float>(localInertia1, localInertia2, localInertia3);
    }

    float CollisionTriangleMeshShape::getMaxDistanceToCenter() const
    {
        throw std::runtime_error("Impossible to get max distance to center for triangle mesh shape. Only static bodies are supported for this shape.");
    }

    float CollisionTriangleMeshShape::getMinDistanceToCenter() const
    {
        return 0.0f;
    }
}
