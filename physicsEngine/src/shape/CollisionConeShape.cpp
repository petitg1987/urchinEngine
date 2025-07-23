#include "shape/CollisionConeShape.h"
#include "object/CollisionConeObject.h"

namespace urchin {

    CollisionConeShape::CollisionConeShape(float radius, float height, ConeShape<float>::ConeOrientation coneOrientation) :
            coneShape(ConeShape(radius, height, coneOrientation)) {
        computeSafeMargin();
    }

    void CollisionConeShape::computeSafeMargin() {
        float minAxis = std::min(getRadius(), getHeight() / 2.0f);
        float maximumMarginPercentage = ConfigService::instance().getFloatValue("collisionShape.maximumMarginPercentage");
        float maximumSafeMargin = minAxis * maximumMarginPercentage;

        refreshInnerMargin(maximumSafeMargin);
    }

    CollisionShape3D::ShapeType CollisionConeShape::getShapeType() const {
        return CONE_SHAPE;
    }

    const ConvexShape3D<float>& CollisionConeShape::getSingleShape() const {
        return coneShape;
    }

    float CollisionConeShape::getRadius() const {
        return coneShape.getRadius();
    }

    float CollisionConeShape::getHeight() const {
        return coneShape.getHeight();
    }

    ConeShape<float>::ConeOrientation CollisionConeShape::getConeOrientation() const {
        return coneShape.getConeOrientation();
    }

    std::unique_ptr<CollisionShape3D> CollisionConeShape::scale(const Vector3<float>& scale) const {
        std::size_t heightAxis = 0;
        if (coneShape.getConeOrientation() == ConeShape<float>::ConeOrientation::CONE_X_POSITIVE || coneShape.getConeOrientation() == ConeShape<float>::ConeOrientation::CONE_X_NEGATIVE) {
            heightAxis = 0;
        } else if (coneShape.getConeOrientation() == ConeShape<float>::ConeOrientation::CONE_Y_POSITIVE || coneShape.getConeOrientation() == ConeShape<float>::ConeOrientation::CONE_Y_NEGATIVE) {
            heightAxis = 1;
        } else if (coneShape.getConeOrientation() == ConeShape<float>::ConeOrientation::CONE_Z_POSITIVE || coneShape.getConeOrientation() == ConeShape<float>::ConeOrientation::CONE_Z_NEGATIVE) {
            heightAxis = 2;
        }

        if (!MathFunction::isEqual(scale[(heightAxis + 1) % 3], scale[(heightAxis + 2) % 3], 0.01f)) {
            Logger::instance().logWarning("Cone cannot be correctly scaled with " + StringUtil::toString(scale) + ". Consider to use another shape.");
        }

        float radiusScale = std::min(scale[(heightAxis + 1) % 3], scale[(heightAxis + 2) % 3]);
        float newRadius = std::max(0.001f, coneShape.getRadius() * radiusScale);
        float newHeight = std::max(0.001f, coneShape.getHeight() * scale[heightAxis]);
        return std::make_unique<CollisionConeShape>(newRadius, newHeight, coneShape.getConeOrientation());
    }

    AABBox<float> CollisionConeShape::toAABBox(const PhysicsTransform& physicsTransform) const {
        Vector3 boxHalfSizes(getRadius(), getRadius(), getRadius());
        boxHalfSizes[getConeOrientation() / 2] = getHeight() / 2.0f;
        Matrix3<float> orientation = physicsTransform.retrieveOrientationMatrix();
        Point3 extend(
                boxHalfSizes.X * std::abs(orientation(0, 0)) + boxHalfSizes.Y * std::abs(orientation(0, 1)) + boxHalfSizes.Z * std::abs(orientation(0, 2)),
                boxHalfSizes.X * std::abs(orientation(1, 0)) + boxHalfSizes.Y * std::abs(orientation(1, 1)) + boxHalfSizes.Z * std::abs(orientation(1, 2)),
                boxHalfSizes.X * std::abs(orientation(2, 0)) + boxHalfSizes.Y * std::abs(orientation(2, 1)) + boxHalfSizes.Z * std::abs(orientation(2, 2))
        );

        const Point3<float>& centerOfMass = physicsTransform.getPosition();
        Vector3 localCentralAxis(0.0f, 0.0f, 0.0f);
        localCentralAxis[getConeOrientation() / 2] = (getConeOrientation() % 2 == 0) ? 1.0f : -1.0f;
        Vector3<float> centralAxis = physicsTransform.getOrientation().rotateVector(localCentralAxis);
        Point3<float> centerPosition = centerOfMass.translate(centralAxis * (1.0f / 4.0f) * getHeight());

        return AABBox(centerPosition - extend, centerPosition + extend);
    }

    std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> CollisionConeShape::toConvexObject(const PhysicsTransform& physicsTransform) const {
        const Point3<float>& position = physicsTransform.getPosition();
        const Quaternion<float>& orientation = physicsTransform.getOrientation();

        float reducedRadius = getRadius() - getInnerMargin();
        float reducedHeight = getHeight() - (2.0f * getInnerMargin());

        void* memPtr = getObjectsPool().allocate(sizeof(CollisionConeObject));
        auto* collisionObjectPtr = new (memPtr) CollisionConeObject(getInnerMargin(), reducedRadius, reducedHeight, getConeOrientation(), position, orientation);
        return std::unique_ptr<CollisionConeObject, ObjectDeleter>(collisionObjectPtr);
    }

    Vector3<float> CollisionConeShape::computeLocalInertia(float mass) const {
        float inertiaValue = (3.0f / 20.0f) * mass * (getRadius() * getRadius() + 4.0f * getHeight() * getHeight());

        Vector3 inertia(inertiaValue, inertiaValue, inertiaValue);
        inertia[getConeOrientation() / 2] = (3.0f / 10.0f) * mass * getRadius() * getRadius();

        return inertia;
    }

    float CollisionConeShape::getMaxDistanceToCenter() const {
        return std::max(coneShape.getHeight() / 2.0f, coneShape.getRadius());
    }

    float CollisionConeShape::getMinDistanceToCenter() const {
        return std::min(coneShape.getHeight() / 2.0f, coneShape.getRadius());
    }

    std::unique_ptr<CollisionShape3D> CollisionConeShape::clone() const {
        return std::make_unique<CollisionConeShape>(coneShape.getRadius(), coneShape.getHeight(), coneShape.getConeOrientation());
    }

}
