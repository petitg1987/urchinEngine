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
		const CSGPolygon *currentPolygon = nullptr;
		unsigned int startPointIndex = findStartPoint(polygon1, polygon2, currentPolygon);
		unsigned int pointIndex = startPointIndex;
		unsigned int nextPointIndex = (startPointIndex+1) % currentPolygon->getCwPoints().size();

		bool foundIntersection = false;

		std::vector<Point2<float>> mergedPolygonPoints;
		mergedPolygonPoints.reserve(polygon1.getCwPoints().size() + polygon2.getCwPoints().size());
		mergedPolygonPoints.push_back(currentPolygon->getCwPoints()[startPointIndex]);

		do
		{
			Point2<float> edgeStartPoint = currentPolygon->getCwPoints()[pointIndex];
			Point2<float> edgeEndPoint = currentPolygon->getCwPoints()[nextPointIndex];

			const CSGPolygon *otherPolygon = currentPolygon==&polygon1 ? &polygon2 : &polygon1;
			CSGIntersection csgIntersection = findFirstIntersectionOnEdge(LineSegment2D<float>(edgeStartPoint, edgeEndPoint), otherPolygon);

			if(csgIntersection.hasIntersection)
			{
				mergedPolygonPoints.push_back(csgIntersection.intersectionPoint);

				currentPolygon = otherPolygon;
				pointIndex = csgIntersection.edgeEndPointIndex;
				nextPointIndex = (pointIndex+1) % currentPolygon->getCwPoints().size();

				foundIntersection = true;
			}else
			{
				mergedPolygonPoints.push_back(edgeEndPoint);

				pointIndex = nextPointIndex;
				nextPointIndex = (pointIndex+1) % currentPolygon->getCwPoints().size();
			}

		}while(nextPointIndex!=startPointIndex);

		std::vector<CSGPolygon> mergedPolygons;
		if(foundIntersection || true) //TODO replace 'true' by: currentPolygon contains otherPolygon
		{
			mergedPolygons.push_back(CSGPolygon(mergedPolygonPoints));
		}else
		{
			mergedPolygons.reserve(2);
			mergedPolygons.push_back(CSGPolygon(polygon1.getCwPoints()));
			mergedPolygons.push_back(CSGPolygon(polygon2.getCwPoints()));
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
		float nearestDistanceEdgeStartPoint = -1.0f;
		Point2<float> nearestIntersectionPoint(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
		unsigned int edgeEndPointIndex;

		const std::vector<Point2<float>> &points = polygon->getCwPoints();
		for(unsigned int i=0; i<points.size(); ++i)
		{
			unsigned int nextI = (i+1)%points.size();
			LineSegment2D<float> polygonEdge(points[i], points[nextI]);

			Point2<float> intersectionPoint = edge.intersectPoint(polygonEdge);
			if(!std::isnan(intersectionPoint.X))
			{ //intersection found
				float distanceEdgeStartPoint = edge.getA().squareDistance(intersectionPoint);
				if(distanceEdgeStartPoint < nearestDistanceEdgeStartPoint)
				{
					nearestDistanceEdgeStartPoint = distanceEdgeStartPoint;
					nearestIntersectionPoint = intersectionPoint;
					edgeEndPointIndex = nextI;
				}
			}
		}

		CSGIntersection csgIntersection;
		csgIntersection.hasIntersection = !std::isnan(nearestIntersectionPoint.X);
		csgIntersection.intersectionPoint = nearestIntersectionPoint;
		csgIntersection.edgeEndPointIndex = edgeEndPointIndex;

		return csgIntersection;
	}

}
