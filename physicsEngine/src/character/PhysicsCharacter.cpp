#include <utility>

#include <character/PhysicsCharacter.h>

namespace urchin {

    PhysicsCharacter::PhysicsCharacter(std::string name, float mass, std::unique_ptr<const CollisionShape3D> shape, const PhysicsTransform& transform) :
            name(std::move(name)),
            mass(mass),
            shape(std::move(shape)),
            transform(transform) {

    }

    const std::string& PhysicsCharacter::getName() const {
        return name;
    }

    float PhysicsCharacter::getMass() const {
        return mass;
    }

    std::unique_ptr<const CollisionShape3D> PhysicsCharacter::moveShape() {
        return std::move(shape);
    }

    void PhysicsCharacter::updateTransform(const PhysicsTransform& transform) {
        this->transform = transform;
    }

    const PhysicsTransform& PhysicsCharacter::getTransform() const {
        return transform;
    }

}
