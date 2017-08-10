#include <cassert>

#include "ResizePolygon2DService.h"

namespace urchin
{

	template<class T> ResizePolygon2DService<T>::ResizePolygon2DService() :
			Singleton<ResizePolygon2DService<T>>()
	{

	}

	template<class T> std::vector<Point2<T>> ResizePolygon2DService<T>::resizePolygon(const std::vector<Point2<T>> &polygonPoints, T distance) const
	{
		std::vector<bool> emptyVector;
		return resizePolygon(polygonPoints, distance, emptyVector);
	}

	/**
	 * @param polygonPoints Points of the polygon which must be sorted in CCW or CW order.
	 * @param distance All edge of polygon will be moved along their normal to the specified distance.
	 * Positive distance will extend polygon if points are sorted in CCW order.
	 * @param edgesAcceptance Indicates which edge/normal is used to move the point.
	 * Edges  are represented by both points which must be true to accept it.
	 */
	template<class T> std::vector<Point2<T>> ResizePolygon2DService<T>::resizePolygon(const std::vector<Point2<T>> &polygonPoints, T distance,
																					  const std::vector<bool> &edgesAcceptance) const
	{
		#ifdef _DEBUG
			assert(edgesAcceptance.empty() || edgesAcceptance.size()==polygonPoints.size());
		#endif

		std::vector<Point2<T>> offsetPoints;
		offsetPoints.reserve(polygonPoints.size());

		for(unsigned int i=0, previousI=polygonPoints.size()-1; i<polygonPoints.size(); previousI=i++)
		{
			unsigned int nextI = (i+1) % polygonPoints.size();

			if(edgesAcceptance.empty() || (edgesAcceptance[previousI] && edgesAcceptance[i] && edgesAcceptance[nextI]))
			{
				Vector2<double> toPreviousPoint = polygonPoints[i].vector(polygonPoints[previousI]).template cast<double>().normalize();
				Vector2<double> firstNormal = Vector2<double>(-toPreviousPoint.Y, toPreviousPoint.X);

				Vector2<double> fromNextPoint = polygonPoints[nextI].vector(polygonPoints[i]).template cast<double>().normalize();
				Vector2<double> secondNormal = Vector2<double>(-fromNextPoint.Y, fromNextPoint.X);

				double moveLength = static_cast<double>(distance) / (1.0 + firstNormal.dotProduct(secondNormal));
				offsetPoints.emplace_back(Point2<T>(
						polygonPoints[i].X + (T) ((firstNormal.X + secondNormal.X) * moveLength),
						polygonPoints[i].Y + (T) ((firstNormal.Y + secondNormal.Y) * moveLength)));
			}else if(edgesAcceptance[i] && edgesAcceptance[nextI])
			{ //point moved along edge [i, nextI] only
				Vector2<double> fromNextPoint = polygonPoints[nextI].vector(polygonPoints[i]).template cast<double>().normalize();
				Vector2<T> normal = Vector2<double>(-fromNextPoint.Y, fromNextPoint.X).template cast<T>();

				offsetPoints.emplace_back(polygonPoints[i].translate(normal * distance));
			}else if(edgesAcceptance[previousI] && edgesAcceptance[i])
			{ //point moved along edge [previousI, i] only
				Vector2<double> toPreviousPoint = polygonPoints[i].vector(polygonPoints[previousI]).template cast<double>().normalize();
				Vector2<T> normal = Vector2<double>(-toPreviousPoint.Y, toPreviousPoint.X).template cast<T>();

				offsetPoints.emplace_back(polygonPoints[i].translate(normal * distance));
			}else
			{ //point not moved
				offsetPoints.emplace_back(polygonPoints[i]);
			}
		}

		return offsetPoints;
	}

	//explicit template
	template class ResizePolygon2DService<float>;

	template class ResizePolygon2DService<double>;

	template class ResizePolygon2DService<int>;

    template class ResizePolygon2DService<long long>;
}
