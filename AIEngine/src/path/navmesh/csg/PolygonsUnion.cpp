#include <algorithm>
#include <cmath>

#include "PolygonsUnion.h"

namespace urchin
{

	PolygonsUnion::PolygonsUnion() :
		epsilon(std::numeric_limits<float>::epsilon())
	{

	}

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

		unsigned int maxIteration = polygon1.getCwPoints().size() + polygon2.getCwPoints().size() * 2;
		unsigned int currentIteration = 0;

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

				edgeStartPoint = edgeEndPoint;
				nextPointIndex = (nextPointIndex+1) % currentPolygon->getCwPoints().size();
			}
		}while(!areSamePoints(startPolygon, startPointIndex, currentPolygon, nextPointIndex) && ++currentIteration < maxIteration);

		std::vector<CSGPolygon> mergedPolygons;
		if(currentIteration >= maxIteration)
		{
			logMaximumIterationReach(polygon1, polygon2);
		}else if(foundIntersection || pointInsideOrOnPolygon(currentPolygon, otherPolygon->getCwPoints()[0]))
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
				if(distanceEdgeStartPoint>epsilon && polygonEdge.getB().squareDistance(intersectionPoint)>epsilon)
				{ //intersection is useful and allow a progression
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

	bool PolygonsUnion::areSamePoints(const CSGPolygon *polygonPoint1, unsigned int point1Index, const CSGPolygon *polygonPoint2, unsigned int point2Index) const
	{
		if(polygonPoint1==polygonPoint2)
		{
			return point1Index==point2Index;
		}

		Point2<float> p1 = polygonPoint1->getCwPoints()[point1Index];
		Point2<float> p2 = polygonPoint2->getCwPoints()[point2Index];

		return ((p1.X-epsilon) <= p2.X) && ((p1.X+epsilon) >= p2.X) && ((p1.Y-epsilon) <= p2.Y) && ((p1.Y+epsilon) >= p2.Y);
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
			{ //TODO check if useful (remove and unit tests)
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

	void PolygonsUnion::logMaximumIterationReach(const CSGPolygon &polygon1, const CSGPolygon &polygon2) const
	{
		std::stringstream logStream;
		logStream<<"Maximum of iteration reached on polygons union algorithm."<<std::endl;
		logStream<<" - Polygon 1: "<<std::endl<<polygon1<<std::endl;
		logStream<<" - Polygon 2: "<<std::endl<<polygon2;
		Logger::logger().logWarning(logStream.str());
	}

}
