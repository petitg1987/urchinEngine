#include "object/CollisionSphereObject.h"

namespace urchin
{

	CollisionSphereObject::CollisionSphereObject(float radius, const Point3<float> &position) :
			CollisionConvexObject3D(radius),
			sphereObject(Sphere<float>(radius, position))
	{

	}

	CollisionSphereObject::~CollisionSphereObject()
	{

	}

	float CollisionSphereObject::getRadius() const
	{
		return getOuterMargin();
	}

	const Point3<float> &CollisionSphereObject::getPosition() const
	{
		return sphereObject.getPosition();
	}

	/**
	 * @return includeMargin Indicate whether support function need to take into account margin
	 */
	Point3<float> CollisionSphereObject::getSupportPoint(const Vector3<float> &direction, bool includeMargin) const
	{
		if(includeMargin)
		{
			return getSphere().getSupportPoint(direction);
		}

		return sphereObject.getPosition();
	}

	const Sphere<float> CollisionSphereObject::getSphere() const
	{
		return Sphere<float>(getRadius(), getPosition());
	}

}
