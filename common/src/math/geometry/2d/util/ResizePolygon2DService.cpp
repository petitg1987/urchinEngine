#include <cassert>

#include "ResizePolygon2DService.h"

namespace urchin
{

	template<class T> ResizePolygon2DService<T>::ResizePolygon2DService() :
			Singleton<ResizePolygon2DService<T>>()
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
		offsetPoints.reserve(polygonPoints.size()); //TODO fix mem alloc

		for(unsigned int i=0, previousI=polygonPoints.size()-1; i<polygonPoints.size(); previousI=i++)
		{
			unsigned int nextI = (i+1) % polygonPoints.size();

			Vector2<double> toPreviousPoint = polygonPoints[i].vector(polygonPoints[previousI]).template cast<double>().normalize();
			Vector2<double> firstNormal = Vector2<double>(-toPreviousPoint.Y, toPreviousPoint.X);

			Vector2<double> fromNextPoint = polygonPoints[nextI].vector(polygonPoints[i]).template cast<double>().normalize();
			Vector2<double> secondNormal = Vector2<double>(-fromNextPoint.Y, fromNextPoint.X);

			double moveLength = static_cast<double>(distance) / (1.0 + firstNormal.dotProduct(secondNormal));
			offsetPoints.emplace_back(Point2<T>(
					polygonPoints[i].X + (T) ((firstNormal.X + secondNormal.X) * moveLength),
					polygonPoints[i].Y + (T) ((firstNormal.Y + secondNormal.Y) * moveLength)));
		}

		return offsetPoints;
	}

	//explicit template
	template class ResizePolygon2DService<float>;

	template class ResizePolygon2DService<double>;

	template class ResizePolygon2DService<int>;

    template class ResizePolygon2DService<long long>;
}
