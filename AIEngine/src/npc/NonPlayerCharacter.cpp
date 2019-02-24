#include "NonPlayerCharacter.h"

#define CHARACTER_MAX_ACCELERATION 1.388889f //take 1 sec go from 0km/h to 5km/h

namespace urchin
{

    NonPlayerCharacter::NonPlayerCharacter(float mass, float maxVelocityInKmH, const Point3<float> &position) :
        mass(mass),
        maxVelocityInKmH(maxVelocityInKmH),
        momentum(Vector3<float>(0.0f, 0.0f, 0.0f)),
        position(position)
    {

    }

    float NonPlayerCharacter::getMass() const
    {
        return mass;
    }

    float NonPlayerCharacter::getMaxVelocityInKmH() const
    {
        return maxVelocityInKmH;
    }

    float NonPlayerCharacter::retrieveMaxVelocityInMs() const
    {
        return (maxVelocityInKmH * 1000.0) / (60.0f * 60.0f);
    }

    /**
     * Returns max force which can be handled by character
     */
    float NonPlayerCharacter::retrieveMaxForce() const
    {
        return mass * CHARACTER_MAX_ACCELERATION;
    }

    float NonPlayerCharacter::retrieveMaxMomentum() const
    {
        return retrieveMaxVelocityInMs() * mass;
    }

    void NonPlayerCharacter::updateMomentum(const Vector3<float> &momentum)
    {
        this->momentum = momentum;
    }

    const Vector3<float> &NonPlayerCharacter::getMomentum() const
    {
        return momentum;
    }

    void NonPlayerCharacter::updatePosition(const Point3<float> &position)
    {
        this->position = position;
    }

    const Point3<float> &NonPlayerCharacter::getPosition() const
    {
        return position;
    }

}
