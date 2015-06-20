#include "math/geometry/2d/object/ConvexHull2D.h"

namespace urchin
{

	/**
	 * @param points Points used to construct the convex hull. Points inside the convex hull are accepted but will unused.
	 */
	template<class T> ConvexHull2D<T>::ConvexHull2D(const std::vector<Point2<T>> &points)
	{ //Monotone Chain algorithm
		unsigned int nbPoints = points.size();
		unsigned int k = 0; //number of points in convex hull

		convexHullPoints.resize(2*nbPoints);

		//sort points lexicographically
		std::vector<Point2<T>> sortedPoints;
		sortedPoints.resize(nbPoints);
		std::copy(points.begin(), points.end(), sortedPoints.begin());
		std::sort(sortedPoints.begin(), sortedPoints.end());

		//build lower convex hull
		for (unsigned int i=0; i<nbPoints; i++)
		{
			while (k>=2 && Line2D<T>(convexHullPoints[k-2], convexHullPoints[k-1]).ccw(sortedPoints[i]) <= 0.0)
			{ //clockwise detected, we remove the point
				k--;
			}
			convexHullPoints[k++] = sortedPoints[i];
		}

		//build upper convex hull
		unsigned int t = k+1;
		for (int i=nbPoints-2; i>=0; i--)
		{
			while (k>=t && Line2D<T>(convexHullPoints[k-2], convexHullPoints[k-1]).ccw(sortedPoints[i]) <= 0.0)
			{ //clockwise detected, we remove the point
				k--;
			}
			convexHullPoints[k++] = sortedPoints[i];
		}

		convexHullPoints.resize(k-1); //k-1: remove the last point which is the same that the first point of lower list
	}

	template<class T> ConvexHull2D<T>::~ConvexHull2D()
	{

	}

	/**
	 * @return Points of the convex hull in clockwise direction
	 */
	template<class T> const std::vector<Point2<T>> &ConvexHull2D<T>::getPoints() const
	{
		return convexHullPoints;
	}

	template<class T> Point2<T> ConvexHull2D<T>::getSupportPoint(const Vector2<T> &direction) const
	{
		T maxPointDotDirection = Point2<T>(0.0, 0.0).vector(convexHullPoints[0]).dotProduct(direction);
		Point2<T> maxPoint = convexHullPoints[0];

		for(unsigned int i=0; i<convexHullPoints.size(); ++i)
		{
			T currentPointDotDirection  = Point2<T>(0.0, 0.0).vector(convexHullPoints[i]).dotProduct(direction);
			if(currentPointDotDirection > maxPointDotDirection)
			{
				maxPointDotDirection = currentPointDotDirection;
				maxPoint = convexHullPoints[i];
			}
		}

		return maxPoint;
	}

	template<class T> T ConvexHull2D<T>::area() const
	{
		T area = 0.0;
		int j;

		for(unsigned int i=0; i<convexHullPoints.size(); i++)
		{
			j = (i + 1) % convexHullPoints.size();
			area += convexHullPoints[i].X * convexHullPoints[j].Y;
			area -= convexHullPoints[i].Y * convexHullPoints[j].X;
		}

		area /= 2.0;
		return std::fabs(area);
	}

	//explicit template
	template class ConvexHull2D<float>;

}
