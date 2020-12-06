#include "object/TemporalObject.h"

namespace urchin {

    TemporalObject::TemporalObject(const CollisionShape3D* shape, const PhysicsTransform& from, const PhysicsTransform& to) :
        shape(shape),
        localObject(shape->toConvexObject(PhysicsTransform())),
        from(from),
        to(to) {

    }

    const CollisionShape3D* TemporalObject::getShape() const {
        return shape;
    }

    const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>& TemporalObject::getLocalObject() const {
        return localObject;
    }

    const PhysicsTransform& TemporalObject::getFrom() const {
        return from;
    }

    const PhysicsTransform& TemporalObject::getTo() const {
        return to;
    }

    bool TemporalObject::isRay() const {
        return shape->getShapeType()==CollisionShape3D::ShapeType::SPHERE_SHAPE && MathFunction::isZero(shape->getInnerMargin());
    }

}



