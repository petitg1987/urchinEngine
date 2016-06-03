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
			return retrieveSphere().getSupportPoint(direction);
		}

		return sphereObject.getPosition();
	}

	const Sphere<float> CollisionSphereObject::retrieveSphere() const
	{
		return Sphere<float>(getRadius(), getPosition());
	}

	std::string CollisionSphereObject::toString() const
	{
		std::stringstream ss;
		ss.precision(std::numeric_limits<float>::max_digits10);

		ss << "Collision sphere:" << std::endl;
		ss << std::setw(20) << std::left << " - Outer margin: " << getOuterMargin() << std::endl;
		ss << std::setw(20) << std::left << " - Radius: " << getRadius() << std::endl;
		ss << std::setw(20) << std::left << " - Position: " << getPosition() << std::endl;

		return ss.str();
	}

}
