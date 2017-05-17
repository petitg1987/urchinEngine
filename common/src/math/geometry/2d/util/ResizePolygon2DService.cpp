#include "ResizePolygon2DService.h"

namespace urchin
{

	template<class T> ResizePolygon2DService<T>::ResizePolygon2DService() :
			Singleton<ResizePolygon2DService<T>>()
	{

	}

	template<class T> ResizePolygon2DService<T>::~ResizePolygon2DService()
	{

	}

	/**
	 * @param polygonPoints Points of the polygon which must be sorted in CCW or CW order.
	 * @param distance All edge of polygon will be moved along their normal to the specified distance.
	 * Positive distance will extend polygon if points are sorted in CCW order.
	 */
	template<class T> std::vector<Point2<T>> ResizePolygon2DService<T>::resizePolygon(const std::vector<Point2<T>> &polygonPoints, T distance) const
	{
		std::vector<Point2<T>> offsetPoints;
		offsetPoints.reserve(polygonPoints.size());

		for(unsigned int i=0, previousI=polygonPoints.size()-1; i<polygonPoints.size(); previousI=i++)
		{
			unsigned int nextI = (i+1) % polygonPoints.size();

			Vector2<T> toPreviousPoint = polygonPoints[i].vector(polygonPoints[previousI]).normalize();
			Vector2<T> firstNormal = Vector2<T>(-toPreviousPoint.Y, toPreviousPoint.X);

			Vector2<T> fromNextPoint = polygonPoints[nextI].vector(polygonPoints[i]).normalize();
			Vector2<T> secondNormal = Vector2<T>(-fromNextPoint.Y, fromNextPoint.X);

			T moveLength = distance / (1.0 + firstNormal.dotProduct(secondNormal));
			offsetPoints.push_back(Point2<T>(
					polygonPoints[i].X + (firstNormal.X + secondNormal.X)*moveLength,
					polygonPoints[i].Y + (firstNormal.Y + secondNormal.Y)*moveLength));
		}

		return offsetPoints;
	}

	//explicit template
	template class ResizePolygon2DService<float>;

	template class ResizePolygon2DService<double>;
}
