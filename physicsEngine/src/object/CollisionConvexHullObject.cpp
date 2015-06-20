#include "object/CollisionConvexHullObject.h"

namespace urchin
{

	/**
	 * @param outerMargin Collision outer margin. Collision margin must match with convex hulls arguments.
	 * @param pointsWithoutMargin Points without margin used to construct the convex hull
	 * @param pointsWithMargin Points with margin used to construct the convex hull
	 */
	CollisionConvexHullObject::CollisionConvexHullObject(float outerMargin, ConvexHull3D<float> convexHullWithMargin, ConvexHull3D<float> convexHullWithoutMargin) :
			CollisionConvexObject3D(outerMargin),
			convexHullObjectWithMargin(convexHullWithMargin),
			convexHullObjectWithoutMargin(convexHullWithoutMargin)
	{

	}

	CollisionConvexHullObject::~CollisionConvexHullObject()
	{

	}

	std::vector<Point3<float>> CollisionConvexHullObject::getPointsWithoutMargin() const
	{
		return convexHullObjectWithoutMargin.getPoints();
	}

	/**
	 * @return includeMargin Indicate whether support function need to take into account margin
	 */
	Point3<float> CollisionConvexHullObject::getSupportPoint(const Vector3<float> &direction, bool includeMargin) const
	{
		if(includeMargin)
		{
			return convexHullObjectWithMargin.getSupportPoint(direction);
		}

		return convexHullObjectWithoutMargin.getSupportPoint(direction);
	}

	const ConvexHull3D<float> CollisionConvexHullObject::getConvexHull() const
	{
		return convexHullObjectWithMargin;
	}

}
