#include <limits>
#include <cassert>

#include "ResizeConvexHull2DService.h"
#include "math/algebra/point/Point2.h"
#include "math/algebra/vector/Vector2.h"

namespace urchin
{

	template<class T> ResizeConvexHull2DService<T>::ResizeConvexHull2DService() :
			Singleton<ResizeConvexHull2DService<T>>()
	{

	}

	template<class T> ResizeConvexHull2DService<T>::~ResizeConvexHull2DService()
	{

	}

	/**
	 * @param distance All edge of convex hull shape will be moved along their normal to the specified distance.
	 * Positive distance will extend convex hull shape. Negative distance are not supported by this algorithm.
	 */
	template<class T> std::unique_ptr<ConvexHullShape2D<T>> ResizeConvexHull2DService<T>::resizeConvexHullShape(const ConvexHullShape2D<T> &originalConvexHullShape, T distance) const
	{
		#ifdef DEBUG
			assert(distance > (0.0 - std::numeric_limits<T>::epsilon()));
		#endif

		const std::vector<Point2<T>> &ccwPoints = originalConvexHullShape.getPoints();

		std::vector<Point2<T>> offsetPoints;
		offsetPoints.reserve(ccwPoints.size());

		for(unsigned int i=0, previousI=ccwPoints.size()-1; i<ccwPoints.size(); previousI=i++)
		{
			unsigned int nextI = (i+1) % ccwPoints.size();

			Vector2<T> toPreviousPoint = ccwPoints[i].vector(ccwPoints[previousI]).normalize();
			Vector2<T> firstNormal = Vector2<T>(-toPreviousPoint.Y, toPreviousPoint.X);

			Vector2<T> fromNextPoint = ccwPoints[nextI].vector(ccwPoints[i]).normalize();
			Vector2<T> secondNormal = Vector2<T>(-fromNextPoint.Y, fromNextPoint.X);

			T moveLength = distance / (1.0 + firstNormal.dotProduct(secondNormal));
			offsetPoints.push_back(Point2<T>(
					ccwPoints[i].X + (firstNormal.X + secondNormal.X)*moveLength,
					ccwPoints[i].Y + (firstNormal.Y + secondNormal.Y)*moveLength));
		}

		return ConvexHullShape2D<T>::createFromCcwConvexPoints(offsetPoints);
	}

	template<class T> std::unique_ptr<ConvexHull2D<T>> ResizeConvexHull2DService<T>::resizeConvexHull(const ConvexHull2D<T> &originalConvexHull, T distance) const
	{
		std::unique_ptr<ConvexHullShape2D<T>> convexHullShapeResized = resizeConvexHullShape(originalConvexHull.localizedConvexHullShape, distance);

		return ConvexHull2D<T>::createFromCcwConvexPoints(convexHullShapeResized->getPoints());
	}

	//explicit template
	template class ResizeConvexHull2DService<float>;

	template class ResizeConvexHull2DService<double>;

}
