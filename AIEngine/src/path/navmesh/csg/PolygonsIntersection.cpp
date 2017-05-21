#include <cmath>

#include "PolygonsIntersection.h"

namespace urchin
{

	PolygonsIntersection::PolygonsIntersection()
	{

	}

	PolygonsIntersection::~PolygonsIntersection()
	{

	}

	CSGPolygon PolygonsIntersection::intersectionPolygons(const CSGPolygon &subjectPolygon, const CSGPolygon &clipPolygon) const
	{ //Sutherland–Hodgman algorithm
		std::vector<Point2<float>> outputList(subjectPolygon.getCwPoints());
		for(unsigned int clipI=0, previousClipI=clipPolygon.getCwPoints().size()-1; clipI<clipPolygon.getCwPoints().size(); previousClipI=clipI++)
		{
			Line2D<float> clipEdge(clipPolygon.getCwPoints()[previousClipI], clipPolygon.getCwPoints()[clipI]);
			std::vector<Point2<float>> inputList(outputList);
			outputList.clear();

			for(unsigned int pointI=0, previousPointI=inputList.size()-1; pointI<inputList.size(); previousPointI=pointI++)
			{
				if(isPointInside(clipEdge, inputList[pointI]))
				{
					if(!isPointInside(clipEdge, inputList[previousPointI]))
					{
						addIntersection(clipEdge, Line2D<float>(inputList[previousPointI], inputList[pointI]), outputList);
					}
					outputList.push_back(inputList[pointI]);
				}else if(isPointInside(clipEdge, inputList[previousPointI]))
				{
					addIntersection(clipEdge, Line2D<float>(inputList[previousPointI], inputList[pointI]), outputList);
				}
			}
		}

		return CSGPolygon(subjectPolygon.getName() + "-clipBy-" + clipPolygon.getName(), outputList);
	}

    bool PolygonsIntersection::isPointInside(const Line2D<float> &line, const Point2<float> &point) const
    {
        return (line.getA().X - point.X) * (line.getB().Y - point.Y) < (line.getA().Y - point.Y) * (line.getB().X - point.X);
    }

	void PolygonsIntersection::addIntersection(const Line2D<float> &line1, const Line2D<float> &line2, std::vector<Point2<float>> &outputList) const
	{
		Point2<float> intersectionPoint = line1.intersectPoint(line2);
		if(!std::isnan(intersectionPoint.X))
		{
			outputList.push_back(intersectionPoint);
		}
	}

}
