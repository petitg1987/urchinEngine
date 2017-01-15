#include <sstream>

#include "CollisionConeObject.h"

namespace urchin
{

	CollisionConeObject::CollisionConeObject(float outerMargin, float radius, float height,
			typename ConeShape<float>::ConeOrientation coneOrientation,	const Point3<float> &centerOfMass, const Quaternion<float> &orientation) :
		CollisionConvexObject3D(outerMargin),
		coneObject(Cone<float>(radius, height, coneOrientation, centerOfMass, orientation))
	{

	}

	CollisionConeObject::~CollisionConeObject()
	{

	}

	/**
	 * @return Radius with margin of the cone
	 */
	float CollisionConeObject::getRadius() const
	{
		return coneObject.getRadius() + getOuterMargin();
	}

	float CollisionConeObject::getHeight() const
	{
		return coneObject.getHeight() + (2.0 * getOuterMargin());
	}

	typename ConeShape<float>::ConeOrientation CollisionConeObject::getConeOrientation() const
	{
		return coneObject.getConeOrientation();
	}

	const Point3<float> &CollisionConeObject::getCenterOfMass() const
	{
		return coneObject.getCenterOfMass();
	}

	const Quaternion<float> &CollisionConeObject::getOrientation() const
	{
		return coneObject.getOrientation();
	}

	/**
	 * @return Cone normalized axis for given index
	 */
	const Vector3<float> &CollisionConeObject::getAxis(unsigned int index) const
	{
		return coneObject.getAxis(index);
	}

	/**
	 * @return includeMargin Indicate whether support function need to take into account margin
	 */
	Point3<float> CollisionConeObject::getSupportPoint(const Vector3<float> &direction, bool includeMargin) const
	{
		if(includeMargin)
		{
			return retrieveCone().getSupportPoint(direction);
		}

		return coneObject.getSupportPoint(direction);
	}

	const Cone<float> CollisionConeObject::retrieveCone() const
	{
		return Cone<float>(getRadius(), getHeight(), getConeOrientation(), getCenterOfMass(), getOrientation());
	}

	std::string CollisionConeObject::toString() const
	{
		std::stringstream ss;
		ss.precision(std::numeric_limits<float>::max_digits10);

		ss << "Collision cone:" << std::endl;
		ss << std::setw(20) << std::left << " - Outer margin: " << getOuterMargin() << std::endl;
		ss << std::setw(20) << std::left << " - Radius: " << coneObject.getRadius() << std::endl;
		ss << std::setw(20) << std::left << " - Height: " << coneObject.getHeight() << std::endl;
		ss << std::setw(20) << std::left << " - Orientation type: " << getConeOrientation() << std::endl;
		ss << std::setw(20) << std::left << " - Center of mass: " << getCenterOfMass() << std::endl;
		ss << std::setw(20) << std::left << " - Orientation: " << getOrientation() << std::endl;

		return ss.str();
	}

}
