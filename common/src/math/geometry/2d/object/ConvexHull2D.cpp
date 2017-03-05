#include "math/geometry/2d/object/ConvexHull2D.h"

namespace urchin
{

	/**
	 * @param points Points used to construct the convex hull. Points inside the convex hull are accepted but will unused.
	 * Duplicate points are supported.
	 */
	template<class T> ConvexHull2D<T>::ConvexHull2D(const std::vector<Point2<T>> &points) :
		localizedConvexHullShape(ConvexHullShape2D<T>(points))
	{

	}

	template<class T> ConvexHull2D<T>::~ConvexHull2D()
	{

	}

	/**
	 * @return Points of the convex hull in counter clockwise direction
	 */
	template<class T> const std::vector<Point2<T>> &ConvexHull2D<T>::getPoints() const
	{
		return localizedConvexHullShape.getPoints();
	}

	template<class T> Point2<T> ConvexHull2D<T>::getSupportPoint(const Vector2<T> &direction) const
	{
		return localizedConvexHullShape.getSupportPoint(direction);
	}

	template<class T> T ConvexHull2D<T>::area() const
	{
		return localizedConvexHullShape.area();
	}

	//explicit template
	template class ConvexHull2D<float>;

}
