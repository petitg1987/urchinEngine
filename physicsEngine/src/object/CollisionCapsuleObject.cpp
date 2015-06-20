#include "object/CollisionCapsuleObject.h"

namespace urchin
{

	CollisionCapsuleObject::CollisionCapsuleObject(float outerMargin, float radius, float cylinderHeight,
			typename CapsuleShape<float>::CapsuleOrientation capsuleOrientation, const Point3<float> &centerPosition, const Quaternion<float> &orientation) :
		CollisionConvexObject3D(outerMargin),
		capsuleObject(Capsule<float>(radius, cylinderHeight, capsuleOrientation, centerPosition, orientation))
	{

	}

	CollisionCapsuleObject::~CollisionCapsuleObject()
	{

	}

	/**
	 * @return Radius with margin of the capsule
	 */
	float CollisionCapsuleObject::getRadius() const
	{
		return capsuleObject.getRadius() + getOuterMargin();
	}

	float CollisionCapsuleObject::getCylinderHeight() const
	{
		return capsuleObject.getCylinderHeight();
	}

	typename CapsuleShape<float>::CapsuleOrientation CollisionCapsuleObject::getCapsuleOrientation() const
	{
		return capsuleObject.getCapsuleOrientation();
	}

	const Point3<float> &CollisionCapsuleObject::getCenterPosition() const
	{
		return capsuleObject.getCenterPosition();
	}

	const Quaternion<float> &CollisionCapsuleObject::getOrientation() const
	{
		return capsuleObject.getOrientation();
	}

	const Vector3<float> &CollisionCapsuleObject::getAxis(unsigned int index) const
	{
		return capsuleObject.getAxis(index);
	}

	/**
	 * @return includeMargin Indicate whether support function need to take into account margin
	 */
	Point3<float> CollisionCapsuleObject::getSupportPoint(const Vector3<float> &direction, bool includeMargin) const
	{
		if(includeMargin)
		{
			return getCapsule().getSupportPoint(direction);
		}

		return capsuleObject.getSupportPoint(direction);
	}

	const Capsule<float> CollisionCapsuleObject::getCapsule() const
	{
		return Capsule<float>(getRadius(), getCylinderHeight(), getCapsuleOrientation(), getCenterPosition(), getOrientation());
	}
}
