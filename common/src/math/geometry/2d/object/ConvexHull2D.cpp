#include "math/geometry/2d/object/ConvexHull2D.h"
#include "math/geometry/2d/util/ResizeConvexHull2DService.h"

namespace urchin
{

	template<class T> ConvexHull2D<T>::ConvexHull2D() :
		localizedConvexHullShape(ConvexHullShape2D<T>())
	{

	}

	/**
	 * @param points Points used to construct the convex hull. Points inside the convex hull are accepted but will unused.
	 * Duplicate points are supported.
	 */
	template<class T> ConvexHull2D<T>::ConvexHull2D(const std::vector<Point2<T>> &points) :
		localizedConvexHullShape(ConvexHullShape2D<T>(points))
	{

	}

	template<class T> std::unique_ptr<ConvexHull2D<T>> ConvexHull2D<T>::createFromCcwConvexPoints(const std::vector<Point2<T>> &ccwConvexPoints)
	{
		std::unique_ptr<ConvexHull2D<T>> convexHull = std::make_unique<ConvexHull2D<T>>();
		convexHull->localizedConvexHullShape = *ConvexHullShape2D<T>::createFromCcwConvexPoints(ccwConvexPoints);

		return convexHull;
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

	template<class T> std::unique_ptr<ConvexHull2D<T>> ConvexHull2D<T>::resize(T distance) const
	{
		return ResizeConvexHull2DService<T>::instance()->resizeConvexHull(*this, distance);
	}

	//explicit template
	template class ConvexHull2D<float>;

	template class ConvexHull2D<double>;

}
