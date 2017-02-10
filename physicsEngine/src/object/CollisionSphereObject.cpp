#include <sstream>

#include "object/CollisionSphereObject.h"

namespace urchin
{

	CollisionSphereObject::CollisionSphereObject(float radius, const Point3<float> &centerOfMass) :
			CollisionConvexObject3D(radius),
			sphereObject(Sphere<float>(radius, centerOfMass))
	{

	}

	CollisionSphereObject::~CollisionSphereObject()
	{

	}

	float CollisionSphereObject::getRadius() const
	{
		return getOuterMargin();
	}

	const Point3<float> &CollisionSphereObject::getCenterOfMass() const
	{
		return sphereObject.getCenterOfMass();
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

		return sphereObject.getCenterOfMass();
	}

	const Sphere<float> CollisionSphereObject::retrieveSphere() const
	{
		return Sphere<float>(getRadius(), getCenterOfMass());
	}

	std::string CollisionSphereObject::toString() const
	{
		std::stringstream ss;
		ss.precision(std::numeric_limits<float>::max_digits10);

		ss << "Collision sphere:" << std::endl;
		ss << std::setw(20) << std::left << " - Outer margin: " << getOuterMargin() << std::endl;
		ss << std::setw(20) << std::left << " - Radius: " << getRadius() << std::endl;
		ss << std::setw(20) << std::left << " - Center of mass: " << getCenterOfMass();

		return ss.str();
	}

}
