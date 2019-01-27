#include "NonPlayerCharacter.h"

namespace urchin
{

    NonPlayerCharacter::NonPlayerCharacter(float mass, const Point3<float> &position) :
        mass(mass),
        momentum(Vector3<float>(0.0f, 0.0f, 0.0f)),
        position(position)
    {

    }

    float NonPlayerCharacter::getMass() const
    {
        return mass;
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
