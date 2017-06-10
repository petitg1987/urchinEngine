#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "PolygonsUnion.h"

namespace urchin
{

    template<class T> PolygonsUnion<T>::PolygonsUnion()
	{

	}

    template<class T> PolygonsUnion<T>::~PolygonsUnion()
	{

	}

    template<class T> std::vector<CSGPolygon<T>> PolygonsUnion<T>::unionPolygons(const std::vector<CSGPolygon<T>> &polygons) const
	{
		std::vector<CSGPolygon<T>> mergedPolygons;
		mergedPolygons.reserve(polygons.size()/2 + 1); //estimated memory size

		std::vector<CSGPolygon<T>> allPolygons = polygons;
		allPolygons.reserve(allPolygons.size() + mergedPolygons.size());

		while(allPolygons.size() > 0)
		{
			bool isPolygonsMerged = false;
			for(unsigned int i=1; i<allPolygons.size(); ++i)
			{
				std::vector<CSGPolygon<T>> result = unionTwoPolygons(allPolygons[0], allPolygons[i]);
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

    template<class T> std::vector<CSGPolygon<T>> PolygonsUnion<T>::unionTwoPolygons(const CSGPolygon<T> &polygon1, const CSGPolygon<T> &polygon2) const
	{
		#ifdef _DEBUG
			//logInputData(polygon1, polygon2, "Debug polygons union.", Logger::INFO);
		#endif

		const CSGPolygon<T> *startPolygon = nullptr;
		unsigned int startPointIndex = findStartPoint(polygon1, polygon2, startPolygon);
		unsigned int nextPointIndex = (startPointIndex+1) % startPolygon->getCwPoints().size();
		unsigned int previousPointIndex = (startPointIndex+startPolygon->getCwPoints().size()-1) % startPolygon->getCwPoints().size();

		const CSGPolygon<T> *currentPolygon = startPolygon;
		const CSGPolygon<T> *otherPolygon = currentPolygon==&polygon1 ? &polygon2 : &polygon1;

		Point2<T> edgeStartPoint = currentPolygon->getCwPoints()[startPointIndex];
        Point2<T> previousEdgeStartPoint = currentPolygon->getCwPoints()[previousPointIndex];
		bool foundIntersection = false;

		unsigned int maxIteration = (polygon1.getCwPoints().size() + polygon2.getCwPoints().size()) * 2;
		unsigned int currentIteration = 0;

		std::vector<Point2<T>> mergedPolygonPoints;
		mergedPolygonPoints.reserve(polygon1.getCwPoints().size() + polygon2.getCwPoints().size()); //estimated memory size
		mergedPolygonPoints.push_back(edgeStartPoint);

		while(currentIteration++ < maxIteration)
		{
			Point2<T> edgeEndPoint = currentPolygon->getCwPoints()[nextPointIndex];
			CSGIntersection<T> csgIntersection = findFirstValidIntersectionOnEdge(LineSegment2D<T>(edgeStartPoint, edgeEndPoint), previousEdgeStartPoint, otherPolygon);

			if(csgIntersection.hasIntersection)
			{
                foundIntersection = true;
				if(areSamePoints(startPolygon->getCwPoints()[startPointIndex], csgIntersection.intersectionPoint))
				{
					break;
				}

				if(mergedPolygonPoints.size()==0 || mergedPolygonPoints[mergedPolygonPoints.size()-1]!=csgIntersection.intersectionPoint)
				{
					mergedPolygonPoints.push_back(csgIntersection.intersectionPoint);

                    previousEdgeStartPoint = edgeStartPoint;
				}

				edgeStartPoint = csgIntersection.intersectionPoint;
				nextPointIndex = csgIntersection.edgeEndPointIndex;

                std::swap(currentPolygon, otherPolygon);
			}else
			{
				if(areSamePoints(startPolygon, startPointIndex, currentPolygon, nextPointIndex))
				{
					break;
				}

				if(mergedPolygonPoints.size()==0 || mergedPolygonPoints[mergedPolygonPoints.size()-1]!=edgeEndPoint)
				{
					mergedPolygonPoints.push_back(edgeEndPoint);

                    previousEdgeStartPoint = edgeStartPoint;
				}
				edgeStartPoint = edgeEndPoint;
				nextPointIndex = (nextPointIndex+1) % currentPolygon->getCwPoints().size();
			}
		}

		std::vector<CSGPolygon<T>> mergedPolygons;
		if(currentIteration >= maxIteration)
		{
			logInputData(polygon1, polygon2, "Maximum of iteration reached on polygons union algorithm.", Logger::ERROR);
		}else if(foundIntersection || pointInsideOrOnPolygon(currentPolygon, otherPolygon->getCwPoints()[0]))
		{
			mergedPolygons.push_back(CSGPolygon<T>(polygon1.getName() + "-" + polygon2.getName(), mergedPolygonPoints));
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
    template<class T> unsigned int PolygonsUnion<T>::findStartPoint(const CSGPolygon<T> &polygon1, const CSGPolygon<T> &polygon2, const CSGPolygon<T> *&startPolygon) const
	{
		unsigned int lowestPointIndexPolygon1 = findLowestPointIndex(polygon1);
		Point2<T> lowestPointPolygon1 = polygon1.getCwPoints()[lowestPointIndexPolygon1];

		unsigned int lowestPointIndexPolygon2 = findLowestPointIndex(polygon2);
		Point2<T> lowestPointPolygon2 = polygon2.getCwPoints()[lowestPointIndexPolygon2];

		if(lowestPointPolygon1.Y == lowestPointPolygon2.Y)
		{
			unsigned int nextPointIndexPolygon1 = (lowestPointIndexPolygon1+1)%polygon1.getCwPoints().size();
			T verticalDot1 = Vector2<T>(0, 1).dotProduct(lowestPointPolygon1.vector(polygon1.getCwPoints()[nextPointIndexPolygon1]).normalize());

			unsigned int nextPointIndexPolygon2 = (lowestPointIndexPolygon2+1)%polygon2.getCwPoints().size();
			T verticalDot2 = Vector2<T>(0, 1).dotProduct(lowestPointPolygon2.vector(polygon2.getCwPoints()[nextPointIndexPolygon2]).normalize());

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

    template<class T> unsigned int PolygonsUnion<T>::findLowestPointIndex(const CSGPolygon<T> &polygon) const
	{
		unsigned int lowestPointIndex = 0;

		const std::vector<Point2<T>> &points = polygon.getCwPoints();
		for(unsigned int i=1; i<points.size(); ++i)
		{
			if(points[i].Y < points[lowestPointIndex].Y)
			{
				lowestPointIndex = i;
			}
		}

		return lowestPointIndex;
	}

    template<class T> CSGIntersection<T> PolygonsUnion<T>::findFirstValidIntersectionOnEdge(const LineSegment2D<T> &edge, const Point2<T> &previousEdgePoint, const CSGPolygon<T> *polygon) const
	{
		CSGIntersection<T> csgIntersection;
		csgIntersection.hasIntersection = false;

		T nearestSquareDistanceEdgeStartPoint = std::numeric_limits<T>::max();

		const std::vector<Point2<T>> &points = polygon->getCwPoints();
		for(unsigned int i=0, previousI=points.size()-1; i<points.size(); previousI=i++)
		{
			LineSegment2D<T> polygonEdge(points[previousI], points[i]);
			bool hasIntersection;
            Point2<T> intersectionPoint = edge.intersectPoint(polygonEdge, hasIntersection);

            if(hasIntersection)
            {
                T squareDistanceEdgeStartPoint = edge.getA().squareDistance(intersectionPoint);
                if (squareDistanceEdgeStartPoint < nearestSquareDistanceEdgeStartPoint && polygonEdge.getB() != intersectionPoint)
                {
                    if ((edge.getA() != intersectionPoint && isIntersectionAngleBetter(edge, intersectionPoint, polygonEdge.getB()))
                        || (edge.getA() == intersectionPoint && isIntersectionAngleBetter(previousEdgePoint, edge, polygonEdge.getB())))
                    {
                        nearestSquareDistanceEdgeStartPoint = squareDistanceEdgeStartPoint;
                        csgIntersection.hasIntersection = true;
                        csgIntersection.intersectionPoint = intersectionPoint;
                        csgIntersection.edgeEndPointIndex = i;
                    }
                }
            }
		}

		return csgIntersection;
	}

    template<class T> bool PolygonsUnion<T>::isIntersectionAngleBetter(const LineSegment2D<T> &edge, const Point2<T> &intersectionPoint, const Point2<T> &nextIntersectionPoint) const
	{
        #ifdef _DEBUG
            if(edge.getA()==intersectionPoint)
            {
                throw std::runtime_error("Vector edge.getA() to intersectionPoint could not be null.");
            }else if(intersectionPoint==nextIntersectionPoint)
            {
                throw std::runtime_error("Vector intersectionPoint to nextIntersectionPoint could not be null.");
            }
        #endif

		Vector2<T> firstVector = edge.getA().vector(intersectionPoint);
		Vector2<T> secondVector = intersectionPoint.vector(nextIntersectionPoint);
		T orientation = firstVector.crossProduct(secondVector);

		if(orientation > 0.0)
		{ //exterior angle is less than 180. Intersection point is a better candidate than edge.getB()
			return true;
		}else if(orientation==0.0)
		{ //angle is 180. check which edge is the most longest
			T lengthEdge = edge.toVector().squareLength();
			T lengthIntersection = edge.getA().vector(nextIntersectionPoint).squareLength();
			return lengthIntersection > lengthEdge;
		}

		//exterior angle is greater than 180. edge.getB() point is a better candidate than intersectionPoint
		return false;
	}

    template<class T> bool PolygonsUnion<T>::isIntersectionAngleBetter(const Point2<T> &previousEdgePoint, const LineSegment2D<T> &edge, const Point2<T> &nextIntersectionPoint) const
    {
        #ifdef _DEBUG
        	if(previousEdgePoint==edge.getA())
			{
				throw std::runtime_error("Previous edge point cannot be equals to edge.getA()");
			}
        #endif

		Vector2<T> previousEdgeVector = LineSegment2D<T>(previousEdgePoint, edge.getA()).toVector();
		T nextEdgeOrientation = previousEdgeVector.crossProduct(edge.toVector());
		T intersectionEdgeOrientation = previousEdgeVector.crossProduct(edge.getA().vector(nextIntersectionPoint));

		if(nextEdgeOrientation>0.0 && intersectionEdgeOrientation<0.0)
		{
			return false;
		}else if(nextEdgeOrientation<0.0 && intersectionEdgeOrientation>0.0)
		{
			return true;
		}

		Vector2<T> normalizedPreviousEdgeVector = previousEdgeVector.normalize();
		T edgeAngle = normalizedPreviousEdgeVector.dotProduct(edge.toVector().normalize());
		T intersectionEdgeAngle = normalizedPreviousEdgeVector.dotProduct(edge.getA().vector(nextIntersectionPoint).normalize());

        if(edgeAngle==intersectionEdgeAngle)
        {
            return edge.getA().vector(nextIntersectionPoint).squareLength() > edge.toVector().squareLength();
        }else if(nextEdgeOrientation <= 0.0)
        {
            return intersectionEdgeAngle > edgeAngle;
        }

        return intersectionEdgeAngle < edgeAngle;
    }

    template<class T> bool PolygonsUnion<T>::areSamePoints(const CSGPolygon<T> *polygonPoint1, unsigned int point1Index, const CSGPolygon<T> *polygonPoint2, unsigned int point2Index) const
	{
		if(polygonPoint1==polygonPoint2)
		{
			return point1Index==point2Index;
		}

		return areSamePoints(polygonPoint1->getCwPoints()[point1Index], polygonPoint2->getCwPoints()[point2Index]);
	}

    template<class T> bool PolygonsUnion<T>::areSamePoints(const Point2<T> &p1, const Point2<T> &p2) const
	{
		constexpr T EPSILON = std::numeric_limits<T>::epsilon(); //TODO remove epsilon
		return ((p1.X-EPSILON) <= p2.X) && ((p1.X+EPSILON) >= p2.X) && ((p1.Y-EPSILON) <= p2.Y) && ((p1.Y+EPSILON) >= p2.Y);
	}

    template<class T> bool PolygonsUnion<T>::pointInsideOrOnPolygon(const CSGPolygon<T> *polygon, const Point2<T> &point) const
	{ //see http://web.archive.org/web/20120323102807/http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
		bool inside = false;

		const std::vector<Point2<T>> &points = polygon->getCwPoints();
		for(unsigned int i=0, previousI=points.size()-1; i<points.size(); previousI=i++)
		{
			Point2<T> point1 = points[previousI];
			Point2<T> point2 = points[i];

			if (((point1.Y<=point.Y && point.Y<point2.Y) || (point2.Y<=point.Y && point.Y<point1.Y))
					&& (point.X < (point2.X-point1.X) * (point.Y-point1.Y) / (point2.Y-point1.Y) + point1.X))
			{
				inside = !inside;
			}else if(LineSegment2D<T>(point1, point2).squareDistance(point)==0.0)
			{
				return true;
			}
		}

		return inside;
	}

    template<class T> void PolygonsUnion<T>::logInputData(const CSGPolygon<T> &polygon1, const CSGPolygon<T> &polygon2, const std::string &message, Logger::CriticalityLevel logLevel) const
	{
		std::stringstream logStream;
		logStream.precision(std::numeric_limits<T>::max_digits10);

		logStream<<message<<std::endl;
		logStream<<" - Polygon 1: "<<std::endl<<polygon1<<std::endl;
		logStream<<" - Polygon 2: "<<std::endl<<polygon2;

		Logger::logger().log(logLevel, logStream.str());
	}

    //explicit template
    template class PolygonsUnion<float>;

    template class PolygonsUnion<double>;

	template class PolygonsUnion<int>;

	template class PolygonsUnion<long long>;
}
