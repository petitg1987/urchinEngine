#include "shape/CollisionTriangleShape.h"

namespace urchin
{

    CollisionTriangleShape::CollisionTriangleShape(const Point3<float> *points) :
            CollisionShape3D(),
            triangleShape(std::make_shared<TriangleShape3D<float>>(points))
    {

    }

    CollisionTriangleShape::CollisionTriangleShape(const Point3<float> &point1, const Point3<float> &point2, const Point3<float> &point3) :
            CollisionShape3D(),
            triangleShape(std::make_shared<TriangleShape3D<float>>(point1, point2, point3))
    {

    }

    CollisionShape3D::ShapeType CollisionTriangleShape::getShapeType() const
    {
        return CollisionShape3D::TRIANGLE_SHAPE;
    }

    std::shared_ptr<ConvexShape3D<float>> CollisionTriangleShape::getSingleShape() const
    {
        return triangleShape;
    }

    std::shared_ptr<CollisionShape3D> CollisionTriangleShape::scale(float scale) const
    {
        return std::make_shared<CollisionTriangleShape>(); //TODO implement it
    }

    AABBox<float> CollisionTriangleShape::toAABBox(const PhysicsTransform &physicsTransform) const
    {
        return AABBox<float>(); //TODO implement it
    }

    std::shared_ptr<CollisionConvexObject3D> CollisionTriangleShape::toConvexObject(const PhysicsTransform &physicsTransform) const
    {
        //TODO implement it
    }

    Vector3<float> CollisionTriangleShape::computeLocalInertia(float mass) const
    {
        //TODO implement it
    }

    float CollisionTriangleShape::getMaxDistanceToCenter() const
    {
        //TODO implement it
    }

    float CollisionTriangleShape::getMinDistanceToCenter() const
    {
        return 0.0;
    }

}
