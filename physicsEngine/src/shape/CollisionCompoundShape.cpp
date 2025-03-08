#include <stdexcept>
#include <limits>

#include <shape/CollisionCompoundShape.h>

namespace urchin {

    CollisionCompoundShape::CollisionCompoundShape(std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedShapes) :
            localizedShapes(std::move(localizedShapes)),
            maxDistanceToCenter(0.0f),
            minDistanceToCenter(0.0f) {
        if (this->localizedShapes.empty()) {
            throw std::invalid_argument("Compound shape must be composed of at least one shape.");
        }

        std::vector<std::size_t> allShapeIndexes;
        allShapeIndexes.reserve(this->localizedShapes.size());
        for (const std::shared_ptr<const LocalizedCollisionShape>& localizedShape : this->localizedShapes) {
            if (std::ranges::any_of(allShapeIndexes, [&localizedShape](std::size_t shapeIndex) {return shapeIndex == localizedShape->shapeIndex;})) {
                throw std::invalid_argument("Found duplicate shape index on localized shapes: " + std::to_string(localizedShape->shapeIndex));
            }
            allShapeIndexes.push_back(localizedShape->shapeIndex);
        }

        initializeDistances();
    }

    void CollisionCompoundShape::initializeDistances() {
        maxDistanceToCenter = toAABBox(PhysicsTransform()).getMaxHalfSize();

        minDistanceToCenter = std::numeric_limits<float>::max();
        for (const auto& localizedShape : localizedShapes) {
            if (localizedShape->shape->getMinDistanceToCenter() < minDistanceToCenter) {
                minDistanceToCenter = localizedShape->shape->getMinDistanceToCenter();
            }
        }
    }

    CollisionShape3D::ShapeType CollisionCompoundShape::getShapeType() const {
        return COMPOUND_SHAPE;
    }

    const ConvexShape3D<float>& CollisionCompoundShape::getSingleShape() const {
        throw std::runtime_error("Impossible to retrieve single convex shape for compound shape");
    }

    const std::vector<std::shared_ptr<const LocalizedCollisionShape>>& CollisionCompoundShape::getLocalizedShapes() const {
        return localizedShapes;
    }

    std::unique_ptr<CollisionShape3D> CollisionCompoundShape::scale(const Vector3<float>& scale) const {
        std::vector<std::shared_ptr<const LocalizedCollisionShape>> scaledLocalizedShapes;
        scaledLocalizedShapes.reserve(localizedShapes.size());
        for (const auto& localizedShape : localizedShapes) {
            auto scaledLocalizedShape = std::make_unique<LocalizedCollisionShape>();
            Point3 scaledPosition(localizedShape->transform.getPosition().X * scale.X, localizedShape->transform.getPosition().Y * scale.Y, localizedShape->transform.getPosition().Z * scale.Z);
            scaledLocalizedShape->shapeIndex = localizedShape->shapeIndex;
            scaledLocalizedShape->shape = localizedShape->shape->scale(scale);
            scaledLocalizedShape->transform = PhysicsTransform(scaledPosition, localizedShape->transform.getOrientation());

            scaledLocalizedShapes.push_back(std::move(scaledLocalizedShape));
        }

        return std::make_unique<CollisionCompoundShape>(std::move(scaledLocalizedShapes));
    }

    AABBox<float> CollisionCompoundShape::toAABBox(const PhysicsTransform& physicsTransform) const {
        PhysicsTransform shapeWorldTransform = physicsTransform * localizedShapes[0]->transform;
        AABBox<float> globalCompoundBox = localizedShapes[0]->shape->toAABBox(shapeWorldTransform);

        for (std::size_t i = 1; i < localizedShapes.size(); ++i) {
            shapeWorldTransform = physicsTransform * localizedShapes[i]->transform;
            AABBox<float> compoundBox = localizedShapes[i]->shape->toAABBox(shapeWorldTransform);

            globalCompoundBox = globalCompoundBox.merge(compoundBox);
        }

        return globalCompoundBox;
    }

    std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> CollisionCompoundShape::toConvexObject(const PhysicsTransform&) const {
        throw std::runtime_error("Impossible to transform compound shape to convex object");
    }

    Vector3<float> CollisionCompoundShape::computeLocalInertia(float mass) const {
        AABBox<float> aabbox = toAABBox(PhysicsTransform());
        float width = 2.0f * aabbox.getHalfSize(0);
        float height = 2.0f * aabbox.getHalfSize(1);
        float depth = 2.0f * aabbox.getHalfSize(2);

        float localInertia1 = (1.0f / 12.0f) * mass * (height * height + depth * depth);
        float localInertia2 = (1.0f / 12.0f) * mass * (width * width + depth * depth);
        float localInertia3 = (1.0f / 12.0f) * mass * (width * width + height * height);
        return Vector3(localInertia1, localInertia2, localInertia3);
    }

    float CollisionCompoundShape::getMaxDistanceToCenter() const {
        return maxDistanceToCenter;
    }

    float CollisionCompoundShape::getMinDistanceToCenter() const {
        return minDistanceToCenter;
    }

    std::unique_ptr<CollisionShape3D> CollisionCompoundShape::clone() const {
        std::vector<std::shared_ptr<const LocalizedCollisionShape>> clonedLocalizedShapes;
        for (const auto& localizedShape : localizedShapes) {
            auto clonedLocalizedShape = std::make_unique<LocalizedCollisionShape>();
            clonedLocalizedShape->shapeIndex = localizedShape->shapeIndex;
            clonedLocalizedShape->shape = localizedShape->shape->clone();
            clonedLocalizedShape->transform = localizedShape->transform;

            clonedLocalizedShapes.push_back(std::move(clonedLocalizedShape));
        }

        return std::make_unique<CollisionCompoundShape>(std::move(clonedLocalizedShapes));
    }

}
