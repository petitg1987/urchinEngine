#include <algorithm>
#include <cmath>

#include "PolygonsUnion.h"

namespace urchin
{

	PolygonsUnion::PolygonsUnion()
	{

	}

	PolygonsUnion::~PolygonsUnion()
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
				if(result.size()==0)
				{ //error: returns empty std::vector
					mergedPolygons.clear();
					return mergedPolygons;
				}else if(result.size()==1)
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
			//logInputData(polygon1, polygon2, "Debug polygons union.", Logger::INFO);
		#endif

		const CSGPolygon *startPolygon = nullptr;
		unsigned int startPointIndex = findStartPoint(polygon1, polygon2, startPolygon);
		unsigned int nextPointIndex = (startPointIndex+1) % startPolygon->getCwPoints().size();

		const CSGPolygon *currentPolygon = startPolygon;
		const CSGPolygon *otherPolygon = currentPolygon==&polygon1 ? &polygon2 : &polygon1;

		Point2<float> edgeStartPoint = currentPolygon->getCwPoints()[startPointIndex];
		bool foundIntersection = false;

		unsigned int maxIteration = (polygon1.getCwPoints().size() + polygon2.getCwPoints().size()) * 2;
		unsigned int currentIteration = 0;

		std::vector<Point2<float>> mergedPolygonPoints;
		mergedPolygonPoints.reserve(polygon1.getCwPoints().size() + polygon2.getCwPoints().size()); //estimated memory size
		mergedPolygonPoints.push_back(edgeStartPoint);

		while(currentIteration++ < maxIteration)
		{
			Point2<float> edgeEndPoint = currentPolygon->getCwPoints()[nextPointIndex];
			Point2<float> nextPointToEdge = currentPolygon->getCwPoints()[(nextPointIndex+1)% currentPolygon->getCwPoints().size()];
			CSGIntersection csgIntersection = findFirstValidIntersectionOnEdge(LineSegment2D<float>(edgeStartPoint, edgeEndPoint), nextPointToEdge, otherPolygon);

			if(csgIntersection.hasIntersection)
			{
				if(areSamePoints(startPolygon->getCwPoints()[startPointIndex], csgIntersection.intersectionPoint))
				{
					break;
				}

				if(mergedPolygonPoints.size()==0 || mergedPolygonPoints[mergedPolygonPoints.size()-1]!=csgIntersection.intersectionPoint)
				{
					mergedPolygonPoints.push_back(csgIntersection.intersectionPoint);
				}
				std::swap(currentPolygon, otherPolygon);

				edgeStartPoint = csgIntersection.intersectionPoint;
				nextPointIndex = csgIntersection.edgeEndPointIndex;

				foundIntersection = true;
			}else
			{
				if(areSamePoints(startPolygon, startPointIndex, currentPolygon, nextPointIndex))
				{
					break;
				}

				if(mergedPolygonPoints.size()==0 || mergedPolygonPoints[mergedPolygonPoints.size()-1]!=edgeEndPoint)
				{
					mergedPolygonPoints.push_back(edgeEndPoint);
				}

				edgeStartPoint = edgeEndPoint;
				nextPointIndex = (nextPointIndex+1) % currentPolygon->getCwPoints().size();
			}
		}

		std::vector<CSGPolygon> mergedPolygons;
		if(currentIteration >= maxIteration)
		{
			logInputData(polygon1, polygon2, "Maximum of iteration reached on polygons union algorithm.", Logger::ERROR);
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
		Point2<float> lowestPointPolygon1 = polygon1.getCwPoints()[lowestPointIndexPolygon1];

		unsigned int lowestPointIndexPolygon2 = findLowestPointIndex(polygon2);
		Point2<float> lowestPointPolygon2 = polygon2.getCwPoints()[lowestPointIndexPolygon2];

		if(lowestPointPolygon1.Y == lowestPointPolygon2.Y)
		{
			unsigned int nextPointIndexPolygon1 = (lowestPointIndexPolygon1+1)%polygon1.getCwPoints().size();
			float verticalDot1 = Vector2<float>(0.0, 1.0).dotProduct(lowestPointPolygon1.vector(polygon1.getCwPoints()[nextPointIndexPolygon1]).normalize());

			unsigned int nextPointIndexPolygon2 = (lowestPointIndexPolygon2+1)%polygon2.getCwPoints().size();
			float verticalDot2 = Vector2<float>(0.0, 1.0).dotProduct(lowestPointPolygon2.vector(polygon2.getCwPoints()[nextPointIndexPolygon2]).normalize());

			if(verticalDot1 < verticalDot2)
			{
				startPolygon = &polygon1;
				return lowestPointIndexPolygon1;
			}
		}else if(lowestPointPolygon1.Y < lowestPointPolygon2.Y)
		{
			startPolygon = &polygon1;
			return lowestPointIndexPolygon1;
		}

		startPolygon = &polygon2;
		return lowestPointIndexPolygon2;
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

	CSGIntersection PolygonsUnion::findFirstValidIntersectionOnEdge(const LineSegment2D<float> &edge, const Point2<float> &nextPointToEdge, const CSGPolygon *polygon) const
	{
		CSGIntersection csgIntersection;
		csgIntersection.hasIntersection = false;

		float nearestSquareDistanceEdgeStartPoint = std::numeric_limits<float>::max();

		const std::vector<Point2<float>> &points = polygon->getCwPoints();
		for(unsigned int i=0, previousI=points.size()-1; i<points.size(); previousI=i++)
		{
			LineSegment2D<float> polygonEdge(points[previousI], points[i]);

			//NOTE: the computation of intersection point is done in 'double' because due to float imprecision,
			//the intersection could be slightly different of the real one and tests below will not work correctly.
			Point2<double> intersectionPoints[2];
			intersectionPoints[0] = edge.cast<double>().intersectPoint(polygonEdge.cast<double>(), intersectionPoints[1]);

			for(unsigned int interIndex=0; interIndex<2; ++interIndex)
			{
				if(!std::isnan(intersectionPoints[interIndex].X) && edge.getA().cast<double>()!=intersectionPoints[interIndex])
				{
					Point2<float> intersectionPoint = intersectionPoints[interIndex].cast<float>();
					float squareDistanceEdgeStartPoint = edge.getA().squareDistance(intersectionPoint);
					if(squareDistanceEdgeStartPoint < nearestSquareDistanceEdgeStartPoint
							&& hasBetterAngleWithIntersectionPoint(edge, nextPointToEdge, polygonEdge.getB())
							&& isExteriorAngleLess180(edge.getA(), intersectionPoint, polygonEdge.getB()))
					{
						nearestSquareDistanceEdgeStartPoint = squareDistanceEdgeStartPoint;
						csgIntersection.hasIntersection = true;
						csgIntersection.intersectionPoint = intersectionPoint;
						csgIntersection.edgeEndPointIndex = i;
						break;
					}
				}
			}
		}

		return csgIntersection;
	}

	bool PolygonsUnion::hasBetterAngleWithIntersectionPoint(const LineSegment2D<float> &edge, const Point2<float> &nextPointToEdge, const Point2<float> &nextIntersectionPoint) const
	{
		if(edge.getB()!=nextIntersectionPoint)
		{
			float nextEdgeOrientation = edge.toVector().crossProduct(edge.getB().vector(nextPointToEdge));
			float intersectionEdgeOrientation = edge.toVector().crossProduct(edge.getB().vector(nextIntersectionPoint));

			if(nextEdgeOrientation>0.0 && intersectionEdgeOrientation<0.0)
			{
				return false;
			}else if(nextEdgeOrientation<0.0 && intersectionEdgeOrientation>0.0)
			{
				return true;
			}

			Vector2<float> normalizedEdgeVector = edge.toVector().normalize();
			float nextEdgeAngle = normalizedEdgeVector.dotProduct(edge.getB().vector(nextPointToEdge).normalize());
			float intersectionEdgeAngle = normalizedEdgeVector.dotProduct(edge.getB().vector(nextIntersectionPoint).normalize());

			return nextEdgeAngle < intersectionEdgeAngle;
		}

		return false;
	}

	bool PolygonsUnion::isExteriorAngleLess180(const Point2<float> &point1, const Point2<float> &point2, const Point2<float> &point3) const
	{
		Vector2<float> firstVector = point1.vector(point2);
		Vector2<float> secondVector = point2.vector(point3);
		return firstVector.crossProduct(secondVector) >= 0.0;
	}

	bool PolygonsUnion::areSamePoints(const CSGPolygon *polygonPoint1, unsigned int point1Index, const CSGPolygon *polygonPoint2, unsigned int point2Index) const
	{
		if(polygonPoint1==polygonPoint2)
		{
			return point1Index==point2Index;
		}

		return areSamePoints(polygonPoint1->getCwPoints()[point1Index], polygonPoint2->getCwPoints()[point2Index]);
	}

	bool PolygonsUnion::areSamePoints(const Point2<float> &p1, const Point2<float> &p2) const
	{
		constexpr float EPSILON = std::numeric_limits<float>::epsilon();
		return ((p1.X-EPSILON) <= p2.X) && ((p1.X+EPSILON) >= p2.X) && ((p1.Y-EPSILON) <= p2.Y) && ((p1.Y+EPSILON) >= p2.Y);
	}

	bool PolygonsUnion::pointInsideOrOnPolygon(const CSGPolygon *polygon, const Point2<float> &point) const
	{ //see http://web.archive.org/web/20120323102807/http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
		bool inside = false;

		const std::vector<Point2<float>> &points = polygon->getCwPoints();
		for(unsigned int i=0, previousI=points.size()-1; i<points.size(); previousI=i++)
		{
			Point2<float> point1 = points[previousI];
			Point2<float> point2 = points[i];

			if (((point1.Y<=point.Y && point.Y<point2.Y) || (point2.Y<=point.Y && point.Y<point1.Y))
					&& (point.X < (point2.X-point1.X) * (point.Y-point1.Y) / (point2.Y-point1.Y) + point1.X))
			{
				inside = !inside;
			}else if(LineSegment2D<float>(point1, point2).squareDistance(point)==0.0)
			{
				return true;
			}
		}

		return inside;
	}

	void PolygonsUnion::logInputData(const CSGPolygon &polygon1, const CSGPolygon &polygon2, const std::string &message, Logger::CriticalityLevel logLevel) const
	{
		std::stringstream logStream;
		logStream.precision(std::numeric_limits<float>::max_digits10);

		logStream<<message<<std::endl;
		logStream<<" - Polygon 1: "<<std::endl<<polygon1<<std::endl;
		logStream<<" - Polygon 2: "<<std::endl<<polygon2;

		Logger::logger().log(logLevel, logStream.str());
	}

}
