#include <sstream>

#include "object/CollisionConvexHullObject.h"

namespace urchin
{

	/**
	 * @param pointsWithMargin Points including margin used to construct the convex hull. Points inside the convex hull are accepted but will unused.
	 * @param pointsWithoutMargin Points without margin used to construct the convex hull. Points inside the convex hull are accepted but will unused.
	 * @param outerMargin Collision outer margin. Collision margin must match with convex hulls arguments.
	 */
	CollisionConvexHullObject::CollisionConvexHullObject(float outerMargin, const std::vector<Point3<float>> &pointsWithMargin, const std::vector<Point3<float>> &pointsWithoutMargin) :
			CollisionConvexObject3D(outerMargin),
			convexHullObjectWithMargin(ConvexHull3D<float>(pointsWithMargin)),
			convexHullObjectWithoutMargin(ConvexHull3D<float>(pointsWithoutMargin))
	{

	}

	/**
	 * @param outerMargin Collision outer margin. Collision margin must match with convex hulls arguments.
	 */
	CollisionConvexHullObject::CollisionConvexHullObject(float outerMargin,
			const std::map<unsigned int, Point3<float>> &indexedPointsWithMargin, const std::map<unsigned int, IndexedTriangle3D<float>> &indexedTrianglesWithMargin,
			const std::map<unsigned int, Point3<float>> &indexedPointsWithoutMargin, const std::map<unsigned int, IndexedTriangle3D<float>> &indexedTrainglesWithoutMargin) :
			CollisionConvexObject3D(outerMargin),
			convexHullObjectWithMargin(ConvexHull3D<float>(indexedPointsWithMargin, indexedTrianglesWithMargin)),
			convexHullObjectWithoutMargin(ConvexHull3D<float>(indexedPointsWithoutMargin, indexedTrainglesWithoutMargin))
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
