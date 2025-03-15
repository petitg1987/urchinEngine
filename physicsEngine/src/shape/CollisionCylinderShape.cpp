#include <shape/CollisionCylinderShape.h>
#include <object/CollisionCylinderObject.h>

namespace urchin {

    CollisionCylinderShape::CollisionCylinderShape(float radius, float height, CylinderShape<float>::CylinderOrientation cylinderOrientation) :
            cylinderShape(CylinderShape(radius, height, cylinderOrientation)) {
        computeSafeMargin();
    }

    void CollisionCylinderShape::computeSafeMargin() {
        float minAxis = std::min(getRadius(), getHeight() / 2.0f);
        float maximumMarginPercentage = ConfigService::instance().getFloatValue("collisionShape.maximumMarginPercentage");
        float maximumSafeMargin = minAxis * maximumMarginPercentage;

        refreshInnerMargin(maximumSafeMargin);
    }

    CollisionShape3D::ShapeType CollisionCylinderShape::getShapeType() const {
        return CYLINDER_SHAPE;
    }

    const ConvexShape3D<float>& CollisionCylinderShape::getSingleShape() const {
        return cylinderShape;
    }

    float CollisionCylinderShape::getRadius() const {
        return cylinderShape.getRadius();
    }

    float CollisionCylinderShape::getHeight() const {
        return cylinderShape.getHeight();
    }

    CylinderShape<float>::CylinderOrientation CollisionCylinderShape::getCylinderOrientation() const {
        return cylinderShape.getCylinderOrientation();
    }

    std::unique_ptr<CollisionShape3D> CollisionCylinderShape::scale(const Vector3<float>& scale) const {
        std::size_t heightAxis = 0;
        if (cylinderShape.getCylinderOrientation() == CylinderShape<float>::CylinderOrientation::CYLINDER_X) {
            heightAxis = 0;
        } else if (cylinderShape.getCylinderOrientation() == CylinderShape<float>::CylinderOrientation::CYLINDER_Y) {
            heightAxis = 1;
        } else if (cylinderShape.getCylinderOrientation() == CylinderShape<float>::CylinderOrientation::CYLINDER_Z) {
            heightAxis = 2;
        }

        if (!MathFunction::isEqual(scale[(heightAxis + 1) % 3], scale[(heightAxis + 2) % 3], 0.01f)) {
            Logger::instance().logWarning("Cylinder cannot be correctly scaled with " + StringUtil::toString(scale) + ". Consider to use another shape.");
        }

        float radiusScale = std::min(scale[(heightAxis + 1) % 3], scale[(heightAxis + 2) % 3]);
        float newRadius = std::max(0.001f, cylinderShape.getRadius() * radiusScale);
        float newHeight = std::max(0.001f, cylinderShape.getHeight() * scale[heightAxis]);
        return std::make_unique<CollisionCylinderShape>(newRadius, newHeight, cylinderShape.getCylinderOrientation());
    }

    AABBox<float> CollisionCylinderShape::toAABBox(const PhysicsTransform& physicsTransform) const {
        Vector3 boxHalfSizes(getRadius(), getRadius(), getRadius());
        boxHalfSizes[getCylinderOrientation()] = getHeight() / 2.0f;
        Matrix3<float> orientation = physicsTransform.retrieveOrientationMatrix();
        Point3<float> extend(
                boxHalfSizes.X * std::abs(orientation(0)) + boxHalfSizes.Y * std::abs(orientation(3)) + boxHalfSizes.Z * std::abs(orientation(6)),
                boxHalfSizes.X * std::abs(orientation(1)) + boxHalfSizes.Y * std::abs(orientation(4)) + boxHalfSizes.Z * std::abs(orientation(7)),
                boxHalfSizes.X * std::abs(orientation(2)) + boxHalfSizes.Y * std::abs(orientation(5)) + boxHalfSizes.Z * std::abs(orientation(8))
        );

        const Point3<float>& position = physicsTransform.getPosition();
        return AABBox(position - extend, position + extend);
    }

    std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> CollisionCylinderShape::toConvexObject(const PhysicsTransform& physicsTransform) const {
        const Point3<float>& position = physicsTransform.getPosition();
        const Quaternion<float>& orientation = physicsTransform.getOrientation();

        float reducedRadius = getRadius() - getInnerMargin();
        float reducedHeight = getHeight() - (2.0f * getInnerMargin());

        void* memPtr = getObjectsPool().allocate(sizeof(CollisionCylinderObject));
        auto* collisionObjectPtr = new (memPtr) CollisionCylinderObject(getInnerMargin(), reducedRadius, reducedHeight, getCylinderOrientation(), position, orientation);
        return std::unique_ptr<CollisionCylinderObject, ObjectDeleter>(collisionObjectPtr);
    }

    Vector3<float> CollisionCylinderShape::computeLocalInertia(float mass) const {
        float inertiaValue = (1.0f / 12.0f) * mass * (3.0f * getRadius() * getRadius() + getHeight() * getHeight());

        Vector3 inertia(inertiaValue, inertiaValue, inertiaValue);
        inertia[getCylinderOrientation()] = 0.5f * mass * getRadius() * getRadius();

        return inertia;
    }

    float CollisionCylinderShape::getMaxDistanceToCenter() const {
        return std::max(cylinderShape.getHeight() / 2.0f, cylinderShape.getRadius());
    }

    float CollisionCylinderShape::getMinDistanceToCenter() const {
        return std::min(cylinderShape.getHeight() / 2.0f, cylinderShape.getRadius());
    }

    std::unique_ptr<CollisionShape3D> CollisionCylinderShape::clone() const {
        return std::make_unique<CollisionCylinderShape>(cylinderShape.getRadius(), cylinderShape.getHeight(), cylinderShape.getCylinderOrientation());
    }

}
