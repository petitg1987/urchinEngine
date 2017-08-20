#include <cmath>

#include "PolygonsIntersection.h"

namespace urchin
{

	/**
	 * Perform an intersection of polygons.
	 * @param clipPolygon Clip polygon must be convex
	 */
	template<class T> CSGPolygon<T> PolygonsIntersection<T>::intersectionPolygons(const CSGPolygon<T> &subjectPolygon, const CSGPolygon<T> &clipPolygon) const
	{ //Sutherland–Hodgman algorithm
		std::vector<Point2<T>> outputList(subjectPolygon.getCwPoints());
		for(unsigned int clipI=0, previousClipI=clipPolygon.getCwPoints().size()-1; clipI<clipPolygon.getCwPoints().size(); previousClipI=clipI++)
		{
			Line2D<T> clipEdge(clipPolygon.getCwPoints()[previousClipI], clipPolygon.getCwPoints()[clipI]);
			std::vector<Point2<T>> inputList(outputList);
			outputList.clear();

			for(unsigned int pointI=0, previousPointI=inputList.size()-1; pointI<inputList.size(); previousPointI=pointI++)
			{
				if(isPointInside(clipEdge, inputList[pointI]))
				{
					if(!isPointInside(clipEdge, inputList[previousPointI]))
					{
						addIntersection(clipEdge, Line2D<T>(inputList[previousPointI], inputList[pointI]), outputList);
					}
					outputList.push_back(inputList[pointI]);
				}else if(isPointInside(clipEdge, inputList[previousPointI]))
				{
					addIntersection(clipEdge, Line2D<T>(inputList[previousPointI], inputList[pointI]), outputList);
				}
			}
		}

		return CSGPolygon<T>("(" + subjectPolygon.getName() + ") ∩ (" + clipPolygon.getName() + ")", outputList);
	}

	template<class T> bool PolygonsIntersection<T>::isPointInside(const Line2D<T> &line, const Point2<T> &point) const
    {
        return (line.getA().X - point.X) * (line.getB().Y - point.Y) < (line.getA().Y - point.Y) * (line.getB().X - point.X);
    }

	template<class T> void PolygonsIntersection<T>::addIntersection(const Line2D<T> &line1, const Line2D<T> &line2, std::vector<Point2<T>> &outputList) const
	{
		bool hasIntersection;
		Point2<T> intersectionPoint = line1.intersectPoint(line2, hasIntersection);
		if(hasIntersection)
		{
			outputList.push_back(intersectionPoint);
		}
	}

	//explicit template
	template class PolygonsIntersection<float>;

	template class PolygonsIntersection<int>;

	template class PolygonsIntersection<long long>;

}
