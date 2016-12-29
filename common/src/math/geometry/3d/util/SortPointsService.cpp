#include <algorithm>

#include "SortPointsService.h"

namespace urchin
{

	template<class T> SortPointsService<T>::SortPointsService() :
		Singleton<SortPointsService<T>>()
	{

	}

	template<class T> SortPointsService<T>::~SortPointsService()
	{

	}

	template<class T> std::vector<Point3<T>> SortPointsService<T>::sortPointsClockwise(const std::vector<Point3<T>> &coplanarPoints, const Vector3<T> &normal)
	{
		return sortPoints(coplanarPoints, normal, SortPointsService::CLOCKWISE);
	}

	template<class T> std::vector<Point3<T>> SortPointsService<T>::sortPointsCounterClockwise(const std::vector<Point3<T>> &coplanarPoints, const Vector3<T> &normal)
	{
		return sortPoints(coplanarPoints, normal, SortPointsService::COUNTER_CLOCKWISE);
	}

	template<class T> std::vector<Point3<T>> SortPointsService<T>::sortPoints(const std::vector<Point3<T>> &coplanarPoints, const Vector3<T> &normal, SortDirection sortDirection)
	{
		std::vector<Point3<T>> sortedPoints;
		sortedPoints.reserve(coplanarPoints.size());

		for(unsigned int i=0; i<2 && i<coplanarPoints.size(); i++)
		{
			sortedPoints.push_back(coplanarPoints[i]);
		}

		for(unsigned int i=2; i<coplanarPoints.size(); ++i)
		{
			unsigned int newPointIndex = sortedPoints.size();
			sortedPoints.push_back(coplanarPoints[i]);
			while(newPointIndex>1)
			{
				if(isNewPointSorted(sortedPoints, normal, newPointIndex, sortDirection))
				{
					break;
				}

				std::swap(sortedPoints[newPointIndex], sortedPoints[newPointIndex-1]);
				--newPointIndex;
			}
		}

		return sortedPoints;
	}

	template<class T> bool SortPointsService<T>::isNewPointSorted(const std::vector<Point3<T>> &points, const Vector3<T> &normal, unsigned int newPointIndex,
			SortDirection sortDirection)
	{
		for(unsigned int i=0; i<3; ++i)
		{ //3 (counter-)clockwise tests: previous point, previous point, new point AND previous point, new point, next point AND new point, next point, next point
			unsigned int firstPointIndex = (newPointIndex-2 + i)%points.size();
			unsigned int secondPointIndex = (newPointIndex-1 + i)%points.size();
			unsigned int thirdPointIndex = (newPointIndex-0 + i)%points.size();

			Vector3<T> vectorFirstSecond = points[firstPointIndex].vector(points[secondPointIndex]);
			Vector3<T> vectorSecondThird = points[secondPointIndex].vector(points[thirdPointIndex]);
			float dotResult = normal.dotProduct(vectorFirstSecond.crossProduct(vectorSecondThird));
			if( (sortDirection==SortDirection::CLOCKWISE && dotResult>0.0f) || (sortDirection==SortDirection::COUNTER_CLOCKWISE && dotResult<0.0f) )
			{ //wrong direction
				return false;
			}
		}

		return true;
	}

	//explicit template
	template class SortPointsService<float>;

	template class SortPointsService<double>;

}
