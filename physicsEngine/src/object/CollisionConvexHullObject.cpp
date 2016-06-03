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

	const ConvexHull3D<float> CollisionConvexHullObject::retrieveConvexHull() const
	{
		return convexHullObjectWithMargin;
	}

	std::string CollisionConvexHullObject::toString() const
	{
		std::stringstream ss;
		ss.precision(std::numeric_limits<float>::max_digits10);

		ss << "Collision convex hull:" << std::endl;
		ss << std::setw(20) << std::left << " - Outer margin: " << getOuterMargin() << std::endl;
		ss << std::setw(20) << std::left << " - Convex hull (margin): " << convexHullObjectWithMargin << std::endl;
		ss << std::setw(20) << std::left << " - Convex hull (no margin): " << convexHullObjectWithoutMargin << std::endl;

		return ss.str();
	}

}
