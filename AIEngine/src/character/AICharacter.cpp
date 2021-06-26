#include <character/AICharacter.h>

#define CHARACTER_MAX_ACCELERATION 1.388889f //take 1 sec go from 0km/h to 5km/h

namespace urchin {

    AICharacter::AICharacter(float mass, float maxVelocityInKmH, const Point3<float>& position) :
        mass(mass),
        maxVelocityInKmH(maxVelocityInKmH),
        velocity(Vector3<float>(0.0f, 0.0f, 0.0f)),
        position(position) {

    }

    float AICharacter::getMass() const {
        return mass;
    }

    float AICharacter::getMaxVelocityInKmH() const {
        return maxVelocityInKmH;
    }

    float AICharacter::retrieveMaxVelocityInMs() const {
        return (maxVelocityInKmH * 1000.0f) / (60.0f * 60.0f);
    }

    /**
     * Returns max force which can be handled by character
     */
    float AICharacter::retrieveMaxForce() const {
        return mass * CHARACTER_MAX_ACCELERATION;
    }

    void AICharacter::updateVelocity(const Vector3<float>& velocity) {
        this->velocity = velocity;
    }

    const Vector3<float>& AICharacter::getVelocity() const {
        return velocity;
    }

    void AICharacter::updatePosition(const Point3<float>& position) {
        this->position = position;
    }

    const Point3<float>& AICharacter::getPosition() const {
        return position;
    }

}
