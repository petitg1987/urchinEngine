#include "shape/CollisionTriangleShape.h"
#include "object/CollisionTriangleObject.h"

namespace urchin
{

    CollisionTriangleShape::CollisionTriangleShape(const Point3<float> *points) :
            CollisionShape3D(),
            triangleShape(std::make_shared<TriangleShape3D<float>>(points)),
            lastConvexObject(nullptr),
            collisionTriangleObjectsPool(nullptr)
    {
        refreshInnerMargin(0.0f); //no margin for triangle
    }

    CollisionTriangleShape::CollisionTriangleShape(const std::shared_ptr<TriangleShape3D<float>> &triangleShape) :
            CollisionShape3D(),
            triangleShape(triangleShape),
            lastConvexObject(nullptr),
            collisionTriangleObjectsPool(nullptr)
    {
        refreshInnerMargin(0.0f); //no margin for triangle
    }

    CollisionTriangleShape::CollisionTriangleShape(CollisionTriangleShape &&collisionTriangleShape) noexcept :
            CollisionShape3D(),
            triangleShape(std::move(collisionTriangleShape.triangleShape)),
            lastConvexObject(std::exchange(collisionTriangleShape.lastConvexObject, nullptr)),
            collisionTriangleObjectsPool(std::exchange(collisionTriangleShape.collisionTriangleObjectsPool, nullptr))
    {
        refreshInnerMargin(0.0f); //no margin for triangle
    }

    CollisionTriangleShape::~CollisionTriangleShape()
    {
        if(collisionTriangleObjectsPool)
        {
            collisionTriangleObjectsPool->free(lastConvexObject);
        }else
        {
            delete lastConvexObject;
        }
    }

    CollisionShape3D::ShapeType CollisionTriangleShape::getShapeType() const
    {
        return CollisionShape3D::TRIANGLE_SHAPE;
    }

    std::shared_ptr<ConvexShape3D<float>> CollisionTriangleShape::getSingleShape() const
    {
        return triangleShape;
    }

    std::shared_ptr<CollisionShape3D> CollisionTriangleShape::scale(float) const
    {
        throw std::runtime_error("Scaling is currently not supported (triangle is only usable as a sub-shape)");
    }

    AABBox<float> CollisionTriangleShape::toAABBox(const PhysicsTransform &physicsTransform) const
    {
        throw std::runtime_error("Retrieving AABBox is currently not supported (triangle is only usable as a sub-shape)");
    }

    void CollisionTriangleShape::setupConvexObjectPool(FixedSizePool<CollisionTriangleObject> *collisionTriangleObjectsPool)
    {
        #ifdef _DEBUG
            assert(lastConvexObject==nullptr);
        #endif

        void *objectMemPtr = collisionTriangleObjectsPool->allocate();
        this->lastConvexObject = new (objectMemPtr) CollisionTriangleObject(0.0, Point3<float>(), Point3<float>(),Point3<float>());
        this->collisionTriangleObjectsPool = collisionTriangleObjectsPool;
    }

    CollisionConvexObject3D *CollisionTriangleShape::toConvexObject(const PhysicsTransform &physicsTransform) const
    {
        #ifdef _DEBUG
            assert(getInnerMargin()==0.0f);
        #endif

        if(lastConvexObject==nullptr)
        {
            lastConvexObject = new CollisionTriangleObject(getInnerMargin(),
                    physicsTransform.transform(triangleShape->getPoints()[0]),
                    physicsTransform.transform(triangleShape->getPoints()[1]),
                    physicsTransform.transform(triangleShape->getPoints()[2]));
            return lastConvexObject;
        }
        return new (lastConvexObject) CollisionTriangleObject(getInnerMargin(),
                physicsTransform.transform(triangleShape->getPoints()[0]),
                physicsTransform.transform(triangleShape->getPoints()[1]),
                physicsTransform.transform(triangleShape->getPoints()[2]));
    }

    Vector3<float> CollisionTriangleShape::computeLocalInertia(float) const
    {
        throw std::runtime_error("Computing local inertia is currently not supported (triangle is only usable as a sub-shape)");
    }

    float CollisionTriangleShape::getMaxDistanceToCenter() const
    {
        throw std::runtime_error("Computing max distance to center is currently not supported (triangle is only usable as a sub-shape)");
    }

    float CollisionTriangleShape::getMinDistanceToCenter() const
    {
        return 0.0;
    }

    CollisionShape3D *CollisionTriangleShape::clone() const
    {
        return new CollisionTriangleShape(triangleShape->getPoints());
    }

}
