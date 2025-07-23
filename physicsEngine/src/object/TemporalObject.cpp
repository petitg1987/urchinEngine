#include "object/TemporalObject.h"

namespace urchin {

    TemporalObject::TemporalObject(const CollisionShape3D& shape, std::size_t shapeIndex, const PhysicsTransform& from, const PhysicsTransform& to) :
            shape(shape),
            shapeIndex(shapeIndex),
            localObject(shape.toConvexObject(PhysicsTransform())),
            from(from),
            to(to) {

    }

    const CollisionShape3D& TemporalObject::getShape() const {
        return shape;
    }

    std::size_t TemporalObject::getShapeIndex() const {
        return shapeIndex;
    }

    const CollisionConvexObject3D& TemporalObject::getLocalObject() const {
        return *localObject;
    }

    const PhysicsTransform& TemporalObject::getFrom() const {
        return from;
    }

    const PhysicsTransform& TemporalObject::getTo() const {
        return to;
    }

    bool TemporalObject::isRay() const {
        return shape.getShapeType() == CollisionShape3D::ShapeType::SPHERE_SHAPE && MathFunction::isZero(shape.getInnerMargin());
    }

}
