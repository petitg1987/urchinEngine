#include <shape/CollisionTriangleShape.h>
#include <object/CollisionTriangleObject.h>

namespace urchin {

    CollisionTriangleShape::CollisionTriangleShape(const std::array<Point3<float>, 3>& points) :
            triangleShape(TriangleShape3D(points)) {
        refreshInnerMargin(0.0f); //no margin for triangle
    }

    CollisionTriangleShape::CollisionTriangleShape(TriangleShape3D<float> triangleShape) :
            triangleShape(std::move(triangleShape)) {
        refreshInnerMargin(0.0f); //no margin for triangle
    }

    CollisionShape3D::ShapeType CollisionTriangleShape::getShapeType() const {
        return TRIANGLE_SHAPE;
    }

    const ConvexShape3D<float>& CollisionTriangleShape::getSingleShape() const {
        return triangleShape;
    }

    std::unique_ptr<CollisionShape3D> CollisionTriangleShape::scale(const Vector3<float>&) const {
        throw std::runtime_error("Scaling is currently not supported (triangle is only usable as a sub-shape)");
    }

    AABBox<float> CollisionTriangleShape::toAABBox(const PhysicsTransform&) const {
        throw std::runtime_error("Retrieving AABBox is currently not supported (triangle is only usable as a sub-shape)");
    }

    std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> CollisionTriangleShape::toConvexObject(const PhysicsTransform& physicsTransform) const {
        assert(getInnerMargin() == 0.0f);

        void* memPtr = getObjectsPool().allocate(sizeof(CollisionTriangleObject));
        auto* collisionObjectPtr = new (memPtr) CollisionTriangleObject(getInnerMargin(),
                physicsTransform.transform(triangleShape.getPoints()[0]),
                physicsTransform.transform(triangleShape.getPoints()[1]),
                physicsTransform.transform(triangleShape.getPoints()[2]));
        return std::unique_ptr<CollisionTriangleObject, ObjectDeleter>(collisionObjectPtr);
    }

    Vector3<float> CollisionTriangleShape::computeLocalInertia(float) const {
        throw std::runtime_error("Computing local inertia is currently not supported (triangle is only usable as a sub-shape)");
    }

    float CollisionTriangleShape::getMaxDistanceToCenter() const {
        throw std::runtime_error("Computing max distance to center is currently not supported (triangle is only usable as a sub-shape)");
    }

    float CollisionTriangleShape::getMinDistanceToCenter() const {
        return 0.0f;
    }

    std::unique_ptr<CollisionShape3D> CollisionTriangleShape::clone() const {
        return std::make_unique<CollisionTriangleShape>(triangleShape.getPoints());
    }

}
