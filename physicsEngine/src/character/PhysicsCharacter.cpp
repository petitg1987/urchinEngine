#include <cmath>
#include <utility>

#include "PhysicsCharacter.h"

#define DEFAULT_JUMP_SPEED 5.0f
#define DEFAULT_MAX_SLOPE MathValue::PI_FLOAT / 4.0f //45 degrees

namespace urchin {

    PhysicsCharacter::PhysicsCharacter(const std::string& name, float mass, const std::shared_ptr<const CollisionShape3D>& shape, const PhysicsTransform& transform) :
        PhysicsCharacter(name, mass, shape, transform, DEFAULT_JUMP_SPEED, DEFAULT_MAX_SLOPE) {

    }

    PhysicsCharacter::PhysicsCharacter(std::string name, float mass, std::shared_ptr<const CollisionShape3D> shape, const PhysicsTransform& transform,
            float jumpSpeed, float maxSlopeInRadian) :
            name(std::move(name)),
            mass(mass),
            shape(std::move(shape)),
            transform(transform),
            jumpSpeed(jumpSpeed),
            maxSlopeInRadian(maxSlopeInRadian),
            maxSlopeInPercentage(std::tan(maxSlopeInRadian)) {

    }

    const std::string& PhysicsCharacter::getName() const {
        return name;
    }

    float PhysicsCharacter::getMass() const {
        return mass;
    }

    const std::shared_ptr<const CollisionShape3D>& PhysicsCharacter::getShape() const {
        return shape;
    }

    void PhysicsCharacter::updateTransform(const PhysicsTransform& transform) {
        std::lock_guard<std::mutex> lock(characterMutex);

        this->transform = transform;
    }

    const PhysicsTransform& PhysicsCharacter::getTransform() const {
        std::lock_guard<std::mutex> lock(characterMutex);

        return transform;
    }

    float PhysicsCharacter::getJumpSpeed() const {
        return jumpSpeed;
    }

    float PhysicsCharacter::getMaxSlopeInRadian() const {
        return maxSlopeInRadian;
    }

    float PhysicsCharacter::getMaxSlopeInPercentage() const {
        return maxSlopeInPercentage;
    }

}
