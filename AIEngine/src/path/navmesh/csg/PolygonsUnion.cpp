#include <algorithm>
#include <cmath>

#include "PolygonsUnion.h"

namespace urchin
{

	std::vector<CSGPolygon> PolygonsUnion::unionPolygons(const std::vector<CSGPolygon> &polygons) const
	{
		std::vector<CSGPolygon> mergedPolygons;
		mergedPolygons.reserve(polygons.size()/2 + 1); //estimated memory size

		std::vector<CSGPolygon> allPolygons = polygons;
		allPolygons.reserve(allPolygons.size() + mergedPolygons.size());

		while(allPolygons.size() > 0)
		{
			bool isPolygonsMerged = false;
			for(unsigned int i=1; i<allPolygons.size(); ++i)
			{
				std::vector<CSGPolygon> result = unionTwoPolygons(allPolygons[0], allPolygons[i]);
				if(result.size()==1)
				{
					isPolygonsMerged = true;

					VectorEraser::erase(allPolygons, i);
					VectorEraser::erase(allPolygons, 0);

					allPolygons.push_back(result[0]);

					break;
				}
			}
			if(!isPolygonsMerged)
			{
				mergedPolygons.push_back(allPolygons[0]);
				VectorEraser::erase(allPolygons, 0);
			}
		}

		return mergedPolygons;
	}

	std::vector<CSGPolygon> PolygonsUnion::unionTwoPolygons(const CSGPolygon &polygon1, const CSGPolygon &polygon2) const
	{
		#ifdef _DEBUG
			//std::cout<<"Polygon 1: "<<polygon1<<std::endl;
			//std::cout<<"Polygon 2: "<<polygon2<<std::endl;
		#endif

		const CSGPolygon *startPolygon = nullptr;
		unsigned int startPointIndex = findStartPoint(polygon1, polygon2, startPolygon);
		unsigned int nextPointIndex = (startPointIndex+1) % startPolygon->getCwPoints().size();

		const CSGPolygon *currentPolygon = startPolygon;
		const CSGPolygon *otherPolygon = currentPolygon==&polygon1 ? &polygon2 : &polygon1;

		Point2<float> edgeStartPoint = currentPolygon->getCwPoints()[startPointIndex];
		bool foundIntersection = false;

		std::vector<Point2<float>> mergedPolygonPoints;
		mergedPolygonPoints.reserve(polygon1.getCwPoints().size() + polygon2.getCwPoints().size());
		mergedPolygonPoints.push_back(edgeStartPoint);

		do
		{
			Point2<float> edgeEndPoint = currentPolygon->getCwPoints()[nextPointIndex];
			CSGIntersection csgIntersection = findFirstIntersectionOnEdge(LineSegment2D<float>(edgeStartPoint, edgeEndPoint), otherPolygon);

			if(csgIntersection.hasIntersection)
			{
				mergedPolygonPoints.push_back(csgIntersection.intersectionPoint);
				std::swap(currentPolygon, otherPolygon);

				edgeStartPoint = csgIntersection.intersectionPoint;
				nextPointIndex = csgIntersection.edgeEndPointIndex;

				foundIntersection = true;
			}else
			{
				mergedPolygonPoints.push_back(edgeEndPoint);

				edgeStartPoint = currentPolygon->getCwPoints()[nextPointIndex];
				nextPointIndex = (nextPointIndex+1) % currentPolygon->getCwPoints().size();
			}
		}while(nextPointIndex!=startPointIndex || currentPolygon!=startPolygon); //TODO probably wrong if only end point is common between 2 polygons

		std::vector<CSGPolygon> mergedPolygons;
		if(foundIntersection || pointInsideOrOnPolygon(currentPolygon, otherPolygon->getCwPoints()[0]))
		{
			mergedPolygons.push_back(CSGPolygon(polygon1.getName() + "-" + polygon2.getName(), mergedPolygonPoints));
		}else
		{
			mergedPolygons.reserve(2);
			mergedPolygons.push_back(polygon1);
			mergedPolygons.push_back(polygon2);
		}

		return mergedPolygons;
	}

	/**
	 * @param startPolygon [OUT] Start polygon to use for merge algorithm
	 * @return Start point to use for merge algorithm
	 */
	unsigned int PolygonsUnion::findStartPoint(const CSGPolygon &polygon1, const CSGPolygon &polygon2, const CSGPolygon *&startPolygon) const
	{
		unsigned int lowestPointIndexPolygon1 = findLowestPointIndex(polygon1);
		float lowestPointPolygon1Value = polygon1.getCwPoints()[lowestPointIndexPolygon1].Y;

		unsigned int lowestPointIndexPolygon2 = findLowestPointIndex(polygon2);
		float lowestPointPolygon2Value = polygon2.getCwPoints()[lowestPointIndexPolygon2].Y;

		startPolygon = lowestPointPolygon1Value < lowestPointPolygon2Value ? &polygon1 : &polygon2;
		return lowestPointPolygon1Value < lowestPointPolygon2Value ? lowestPointIndexPolygon1 : lowestPointIndexPolygon2;
	}

	unsigned int PolygonsUnion::findLowestPointIndex(const CSGPolygon &polygon) const
	{
		unsigned int lowestPointIndex = 0;

		const std::vector<Point2<float>> &points = polygon.getCwPoints();
		for(unsigned int i=1; i<points.size(); ++i)
		{
			if(points[i].Y < points[lowestPointIndex].Y)
			{
				lowestPointIndex = i;
			}
		}

		return lowestPointIndex;
	}

	CSGIntersection PolygonsUnion::findFirstIntersectionOnEdge(const LineSegment2D<float> &edge, const CSGPolygon *polygon) const
	{
		float nearestDistanceEdgeStartPoint = std::numeric_limits<float>::max();
		Point2<float> nearestIntersectionPoint(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
		unsigned int edgeEndPointIndex;

		const std::vector<Point2<float>> &points = polygon->getCwPoints();
		for(unsigned int i=0, previousI=points.size()-1; i<points.size(); previousI=i++)
		{
			LineSegment2D<float> polygonEdge(points[previousI], points[i]);

			Point2<float> intersectionPoint = edge.intersectPoint(polygonEdge);
			if(!std::isnan(intersectionPoint.X))
			{ //intersection found
				float distanceEdgeStartPoint = edge.getA().squareDistance(intersectionPoint);
				float distanceEdgeEndPoint = edge.getB().squareDistance(intersectionPoint);
				if(distanceEdgeStartPoint > std::numeric_limits<float>::epsilon() && distanceEdgeEndPoint > std::numeric_limits<float>::epsilon())
				{ //as edge.getA/B() could be an intersection point, we don't want to find it again
					if(distanceEdgeStartPoint < nearestDistanceEdgeStartPoint)
					{
						nearestDistanceEdgeStartPoint = distanceEdgeStartPoint;
						nearestIntersectionPoint = intersectionPoint;
						edgeEndPointIndex = i;
					}
				}
			}
		}

		CSGIntersection csgIntersection;
		csgIntersection.hasIntersection = !std::isnan(nearestIntersectionPoint.X);
		csgIntersection.intersectionPoint = nearestIntersectionPoint;
		csgIntersection.edgeEndPointIndex = edgeEndPointIndex;

		return csgIntersection;
	}

	bool PolygonsUnion::pointInsideOrOnPolygon(const CSGPolygon *polygon, const Point2<float> &point) const
	{ //see http://web.archive.org/web/20120323102807/http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
		bool inside = false;

		const std::vector<Point2<float>> &points = polygon->getCwPoints();
		for(unsigned int i=0, previousI=points.size()-1; i<points.size(); previousI=i++)
		{
			Point2<float> point1 = points[previousI];
			Point2<float> point2 = points[i];

			if(point1.X==point.X && point1.Y==point.Y)
			{
				return true;
			}

			if (((point1.Y<=point.Y && point.Y<point2.Y) || (point2.Y<=point.Y && point.Y<point1.Y))
					&& (point.X < (point2.X-point1.X) * (point.Y-point1.Y) / (point2.Y-point1.Y) + point1.X))
			{
				inside = !inside;
			}
		}

		return inside;
	}

}
