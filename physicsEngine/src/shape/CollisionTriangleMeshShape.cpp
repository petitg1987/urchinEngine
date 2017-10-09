#include "CollisionTriangleMeshShape.h"

namespace urchin
{

    CollisionTriangleMeshShape::CollisionTriangleMeshShape() :
            CollisionShape3D()
    {

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
        //TODO complete
    }

    AABBox<float> CollisionTriangleMeshShape::toAABBox(const PhysicsTransform &physicsTransform) const
    {
        //TODO complete
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
        //TODO complete
    }

    float CollisionTriangleMeshShape::getMinDistanceToCenter() const
    {
        //TODO complete
    }
}
