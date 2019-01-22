#include "NonPlayerCharacter.h"

namespace urchin
{

    NonPlayerCharacter::NonPlayerCharacter(const Point3<float> &position) :
        velocity(Vector3<float>(0.0f, 0.0f, 0.0f)),
        position(position)
    {

    }

    void NonPlayerCharacter::setVelocity(const Vector3<float> &velocity)
    {
        this->velocity = velocity;
    }

    const Vector3<float> &NonPlayerCharacter::getVelocity() const
    {
        return velocity;
    }

    void NonPlayerCharacter::setPosition(const Point3<float> &position)
    {
        this->position = position;
    }

    const Point3<float> &NonPlayerCharacter::getPosition() const
    {
        return position;
    }

}
