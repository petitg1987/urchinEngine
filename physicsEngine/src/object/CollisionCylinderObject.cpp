#include "object/CollisionCylinderObject.h"

namespace urchin
{

	CollisionCylinderObject::CollisionCylinderObject(float outerMargin, float radius, float height,
			typename CylinderShape<float>::CylinderOrientation cylinderOrientation,	const Point3<float> &centerPosition, const Quaternion<float> &orientation) :
		CollisionConvexObject3D(outerMargin),
		cylinderObject(Cylinder<float>(radius, height, cylinderOrientation, centerPosition, orientation))
	{

	}

	CollisionCylinderObject::~CollisionCylinderObject()
	{

	}

	/**
	 * @return Radius with margin of the cylinder
	 */
	float CollisionCylinderObject::getRadius() const
	{
		return cylinderObject.getRadius() + getOuterMargin();
	}

	float CollisionCylinderObject::getHeight() const
	{
		return cylinderObject.getHeight() + (2.0 * getOuterMargin());
	}

	typename CylinderShape<float>::CylinderOrientation CollisionCylinderObject::getCylinderOrientation() const
	{
		return cylinderObject.getCylinderOrientation();
	}

	const Point3<float> &CollisionCylinderObject::getCenterPosition() const
	{
		return cylinderObject.getCenterPosition();
	}

	const Quaternion<float> &CollisionCylinderObject::getOrientation() const
	{
		return cylinderObject.getOrientation();
	}

	/**
	 * @return Cylinder normalized axis for given index
	 */
	const Vector3<float> &CollisionCylinderObject::getAxis(unsigned int index) const
	{
		return cylinderObject.getAxis(index);
	}

	/**
	 * @return includeMargin Indicate whether support function need to take into account margin
	 */
	Point3<float> CollisionCylinderObject::getSupportPoint(const Vector3<float> &direction, bool includeMargin) const
	{
		if(includeMargin)
		{
			return retrieveCylinder().getSupportPoint(direction);
		}

		return cylinderObject.getSupportPoint(direction);
	}

	const Cylinder<float> CollisionCylinderObject::retrieveCylinder() const
	{
		return Cylinder<float>(getRadius(), getHeight(), getCylinderOrientation(), getCenterPosition(), getOrientation());
	}
}
