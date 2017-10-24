#include "shape/CollisionTriangleShape.h"
#include "object/CollisionTriangleObject.h"

namespace urchin
{

    /**
     * Build a collision triangle shape where center is origin (0.0, 0.0, 0.0)
     */
    CollisionTriangleShape::CollisionTriangleShape(const Point3<float> *points) :
            CollisionShape3D(),
            triangleShape(std::make_shared<TriangleShape3D<float>>(points))
    {
        //TODO check if margin can be added for terrain !
        refreshInnerMargin(0.0f); //no margin for triangle
    }

    /**
     * Build a collision triangle shape where center is origin (0.0, 0.0, 0.0)
     */
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
        throw std::runtime_error("Scaling is currently not supported (triangle is only usable as a sub-shape)");
    }

    AABBox<float> CollisionTriangleShape::toAABBox(const PhysicsTransform &physicsTransform) const
    {
        throw std::runtime_error("Retrieving AABBox is currently not supported (triangle is only usable as a sub-shape)");
    }

    std::shared_ptr<CollisionConvexObject3D> CollisionTriangleShape::toConvexObject(const PhysicsTransform &physicsTransform) const
    {
        #ifdef _DEBUG
            assert(getInnerMargin()==0.0f);
        #endif

        return std::make_shared<CollisionTriangleObject>(getInnerMargin(),
                physicsTransform.transform(triangleShape->getPoints()[0]),
                physicsTransform.transform(triangleShape->getPoints()[1]),
                physicsTransform.transform(triangleShape->getPoints()[2]));
    }

    Vector3<float> CollisionTriangleShape::computeLocalInertia(float mass) const
    {
        throw std::runtime_error("Computing local inertia is currently not supported (triangle is only usable as a sub-shape)");
    }

    float CollisionTriangleShape::getMaxDistanceToCenter() const
    {
        float point1ToCenter = triangleShape->getPoints()[0].vector(Point3<float>(0.0, 0.0, 0.0)).length();
        float point2ToCenter = triangleShape->getPoints()[1].vector(Point3<float>(0.0, 0.0, 0.0)).length();
        float point3ToCenter = triangleShape->getPoints()[2].vector(Point3<float>(0.0, 0.0, 0.0)).length();

        if(point1ToCenter > point2ToCenter)
        {
            if(point1ToCenter > point3ToCenter)
            {
                return point1ToCenter;
            }
        }else if(point2ToCenter > point3ToCenter)
        {
            return point2ToCenter;
        }
        return point3ToCenter;
    }

    float CollisionTriangleShape::getMinDistanceToCenter() const
    {
        return 0.0;
    }

}
