#include <utility>
#include <stdexcept>
#include <string>

#include "character/PhysicsCharacter.h"
#include "shape/CollisionCapsuleShape.h"

namespace urchin {

    PhysicsCharacter::PhysicsCharacter(std::string name, float mass, std::unique_ptr<const CollisionShape3D> standingShape, const PhysicsTransform& transform, float crouchHeight) :
            name(std::move(name)),
            mass(mass),
            standingShape(std::move(standingShape)),
            crouchHeight(crouchHeight),
            transform(transform) {
        if (isCrouchEnabled()) {
            if (this->standingShape->getShapeType() != CollisionShape3D::ShapeType::CAPSULE_SHAPE) {
                throw std::invalid_argument("Crouch is only supported for capsule shape: " + std::to_string(this->standingShape->getShapeType()));
            }
            const auto& standingCapsuleShape = static_cast<const CollisionCapsuleShape&>(*this->standingShape);
            float diameter = 2.0f * standingCapsuleShape.getRadius();
            float height = standingCapsuleShape.getCylinderHeight() + diameter;
            if (crouchHeight >= height || crouchHeight <= diameter) {
                throw std::invalid_argument("Crouch height (" + std::to_string(crouchHeight) + ") must be lower than the character height (" + std::to_string(height)
                        + ") and higher than the character diameter (" + std::to_string(diameter) + ")");
            }
        }
    }

    const std::string& PhysicsCharacter::getName() const {
        return name;
    }

    float PhysicsCharacter::getMass() const {
        return mass;
    }

    const CollisionShape3D& PhysicsCharacter::getStandingShape() const {
        return *standingShape;
    }

    float PhysicsCharacter::getCrouchHeight() const {
        return crouchHeight;
    }

    bool PhysicsCharacter::isCrouchEnabled() const {
        return crouchHeight > 0.0f;
    }

    void PhysicsCharacter::updateTransform(const PhysicsTransform& transform) {
        this->transform = transform;
    }

    const PhysicsTransform& PhysicsCharacter::getTransform() const {
        return transform;
    }

}
