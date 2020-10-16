#include <stdexcept>
#include <limits>

#include "shape/CollisionCompoundShape.h"

namespace urchin {

    CollisionCompoundShape::CollisionCompoundShape(const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &localizedShapes) :
            CollisionShape3D(),
            localizedShapes(localizedShapes),
            maxDistanceToCenter(0.0f),
            minDistanceToCenter(0.0f) {
        if (localizedShapes.empty()) {
            throw std::invalid_argument("Compound shape must be composed of at least one shape.");
        }

        initializeDistances();
    }

    void CollisionCompoundShape::initializeDistances() {
        maxDistanceToCenter = toAABBox(PhysicsTransform()).getMaxHalfSize();

        minDistanceToCenter = std::numeric_limits<float>::max();
        for (const auto &localizedShape : localizedShapes) {
            if (localizedShape->shape->getMinDistanceToCenter() < minDistanceToCenter) {
                minDistanceToCenter = localizedShape->shape->getMinDistanceToCenter();
            }
        }
    }

    CollisionShape3D::ShapeType CollisionCompoundShape::getShapeType() const {
        return CollisionShape3D::COMPOUND_SHAPE;
    }

    const ConvexShape3D<float> *CollisionCompoundShape::getSingleShape() const {
        throw std::runtime_error("Impossible to retrieve single convex shape for compound shape");
    }

    const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &CollisionCompoundShape::getLocalizedShapes() const {
        return localizedShapes;
    }

    std::shared_ptr<CollisionShape3D> CollisionCompoundShape::scale(float scale) const {
        std::vector<std::shared_ptr<const LocalizedCollisionShape>> scaledLocalizedShapes;
        scaledLocalizedShapes.reserve(localizedShapes.size());
        for (const auto &localizedShape : localizedShapes) {
            std::shared_ptr<LocalizedCollisionShape> scaledLocalizedShape = std::make_shared<LocalizedCollisionShape>();
            scaledLocalizedShape->position = localizedShape->position;
            scaledLocalizedShape->shape = localizedShape->shape->scale(scale);
            scaledLocalizedShape->transform = PhysicsTransform(localizedShape->transform.getPosition() * scale, localizedShape->transform.getOrientation());

            scaledLocalizedShapes.push_back(scaledLocalizedShape);
        }

        return std::make_shared<CollisionCompoundShape>(scaledLocalizedShapes);
    }

    AABBox<float> CollisionCompoundShape::toAABBox(const PhysicsTransform &physicsTransform) const {
        if (!lastTransform.equals(physicsTransform)) {
            PhysicsTransform shapeWorldTransform = physicsTransform * localizedShapes[0]->transform;
            AABBox<float> globalCompoundBox = localizedShapes[0]->shape->toAABBox(shapeWorldTransform);

            for (std::size_t i = 1; i < localizedShapes.size(); ++i) {
                shapeWorldTransform = physicsTransform * localizedShapes[i]->transform;
                AABBox<float> compoundBox = localizedShapes[i]->shape->toAABBox(shapeWorldTransform);

                globalCompoundBox = globalCompoundBox.merge(compoundBox);
            }

            lastAABBox = globalCompoundBox;
            lastTransform = physicsTransform;
        }

        return lastAABBox;
    }

    std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> CollisionCompoundShape::toConvexObject(const PhysicsTransform &) const {
        throw std::runtime_error("Impossible to transform compound shape to convex object");
    }

    Vector3<float> CollisionCompoundShape::computeLocalInertia(float mass) const {
        AABBox<float> aabbox = toAABBox(PhysicsTransform());
        float width = 2.0f * aabbox.getHalfSize(0);
        float height = 2.0f * aabbox.getHalfSize(1);
        float depth = 2.0f * aabbox.getHalfSize(2);

        float localInertia1 = (1.0f/12.0f) * mass * (height*height + depth*depth);
        float localInertia2 = (1.0f/12.0f) * mass * (width*width + depth*depth);
        float localInertia3 = (1.0f/12.0f) * mass * (width*width + height*height);
        return Vector3<float>(localInertia1, localInertia2, localInertia3);
    }

    float CollisionCompoundShape::getMaxDistanceToCenter() const {
        return maxDistanceToCenter;
    }

    float CollisionCompoundShape::getMinDistanceToCenter() const {
        return minDistanceToCenter;
    }

    CollisionShape3D *CollisionCompoundShape::clone() const {
        std::vector<std::shared_ptr<const LocalizedCollisionShape>> clonedLocalizedShapes;
        for (const auto &localizedShape : localizedShapes) {
            auto clonedLocalizedShape = std::make_shared<LocalizedCollisionShape>();
            clonedLocalizedShape->position = localizedShape->position;
            clonedLocalizedShape->shape = std::shared_ptr<const CollisionShape3D>(localizedShape->shape->clone());
            clonedLocalizedShape->transform = localizedShape->transform;

            clonedLocalizedShapes.push_back(clonedLocalizedShape);
        }

        return new CollisionCompoundShape(clonedLocalizedShapes);
    }

}
