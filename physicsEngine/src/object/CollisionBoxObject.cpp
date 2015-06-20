#include "object/CollisionBoxObject.h"

namespace urchin
{

	CollisionBoxObject::CollisionBoxObject(float outerMargin, const Vector3<float> &halfSize, const Point3<float> &centerPosition, const Quaternion<float> &orientation) :
			CollisionConvexObject3D(outerMargin),
			boxObject(OBBox<float>(halfSize, centerPosition, orientation))
	{

	}

	CollisionBoxObject::~CollisionBoxObject()
	{

	}

	float CollisionBoxObject::getHalfSize(unsigned int index) const
	{
		return boxObject.getHalfSize(index) + getOuterMargin();
	}

	Vector3<float> CollisionBoxObject::getHalfSizes() const
	{
		return boxObject.getHalfSizes() + Vector3<float>(getOuterMargin(), getOuterMargin(), getOuterMargin());
	}

	const Point3<float> &CollisionBoxObject::getCenterPosition() const
	{
		return boxObject.getCenterPosition();
	}

	const Quaternion<float> &CollisionBoxObject::getOrientation() const
	{
		return boxObject.getOrientation();
	}

	const Vector3<float> &CollisionBoxObject::getAxis(unsigned int index) const
	{
		return boxObject.getAxis(index);
	}

	/**
	 * @return includeMargin Indicate whether support function need to take into account margin
	 */
	Point3<float> CollisionBoxObject::getSupportPoint(const Vector3<float> &direction, bool includeMargin) const
	{
		if(includeMargin)
		{
			const Point3<float> &supportPoint = boxObject.getSupportPoint(direction);
			Point3<float> supportPointWithMargin = supportPoint;

			for(unsigned int i=0; i<3; ++i)
			{ //for each axis
				const Vector3<float> &axis = boxObject.getAxis(i);
				if(axis.dotProduct(boxObject.getCenterPosition().vector(supportPoint)) > 0.0f)
				{
					supportPointWithMargin = supportPointWithMargin.translate(axis * getOuterMargin());
				}else
				{
					supportPointWithMargin = supportPointWithMargin.translate(-(axis * getOuterMargin()));
				}
			}

			return supportPointWithMargin;
		}

		return boxObject.getSupportPoint(direction);
	}

	const OBBox<float> CollisionBoxObject::getOBBox() const
	{
		return OBBox<float>(getHalfSizes(), getCenterPosition(), getOrientation());
	}

}
