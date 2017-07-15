#include <algorithm>
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <set>

#include "PolygonsUnion.h"

namespace urchin
{

    template<class T> PolygonsUnion<T>::PolygonsUnion()
	{

	}

    template<class T> PolygonsUnion<T>::~PolygonsUnion()
	{

	}

	/**
  	 * Perform an union of polygons.
  	 * When polygons cannot be put together because there is no contact: there are returned apart.
  	 */
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
        unsigned int currentPolygonEdgeId = startPointIndex;

		const CSGPolygon<T> *currentPolygon = startPolygon;
		const CSGPolygon<T> *otherPolygon = currentPolygon==&polygon1 ? &polygon2 : &polygon1;

		Point2<T> edgeStartPoint = currentPolygon->getCwPoints()[startPointIndex];
        Point2<T> previousEdgeStartPoint = currentPolygon->getCwPoints()[previousPointIndex];
		bool foundIntersection = false;

		unsigned int maxIteration = (polygon1.getCwPoints().size() + polygon2.getCwPoints().size()) * 2;
		unsigned int currentIteration = 0;

        std::set<uint_fast64_t> intersectionsProcessed;
		std::vector<Point2<T>> mergedPolygonPoints;
		mergedPolygonPoints.reserve(polygon1.getCwPoints().size() + polygon2.getCwPoints().size()); //estimated memory size
		mergedPolygonPoints.push_back(edgeStartPoint);

		while(currentIteration++ < maxIteration+1)
		{
            LineSegment2D<T> edge(edgeStartPoint, currentPolygon->getCwPoints()[nextPointIndex]);
			CSGIntersection<T> csgIntersection = findFirstIntersectionOnEdge(edge, previousEdgeStartPoint, otherPolygon, currentPolygon,
                                                                             currentPolygonEdgeId, intersectionsProcessed);
            Point2<T> nextUnionPoint = csgIntersection.hasIntersection ? csgIntersection.intersectionPoint : edge.getB();

            if(areSamePoints(startPolygon->getCwPoints()[startPointIndex], nextUnionPoint) && mergedPolygonPoints.size() > 1)
            {
                break;
            }

            if(csgIntersection.hasIntersection)
            {
				foundIntersection = true;
                if(mergedPolygonPoints.size()==0 || mergedPolygonPoints[mergedPolygonPoints.size()-1]!=nextUnionPoint)
                {
                    mergedPolygonPoints.push_back(nextUnionPoint);
                    previousEdgeStartPoint = edgeStartPoint;

                    intersectionsProcessed.insert(csgIntersection.intersectionId);
                }

                edgeStartPoint = nextUnionPoint;
                currentPolygonEdgeId = csgIntersection.edgeStartPointIndex;
                nextPointIndex = csgIntersection.edgeEndPointIndex;
                std::swap(currentPolygon, otherPolygon);
            }else
            {
                if(mergedPolygonPoints.size()==0 || mergedPolygonPoints[mergedPolygonPoints.size()-1]!=nextUnionPoint)
                {
                    mergedPolygonPoints.push_back(nextUnionPoint);
                    previousEdgeStartPoint = edgeStartPoint;
                }

                edgeStartPoint = nextUnionPoint;
                currentPolygonEdgeId = nextPointIndex;
                nextPointIndex = (nextPointIndex+1) % currentPolygon->getCwPoints().size();
            }
		}

		std::vector<CSGPolygon<T>> mergedPolygons;
		if(currentIteration > maxIteration)
		{
			logInputData(polygon1, polygon2, "Maximum of iteration reached on polygons union algorithm.", Logger::ERROR);
		}else if(foundIntersection || currentPolygon->pointInsideOrOnPolygon(otherPolygon->getCwPoints()[0]))
		{
			mergedPolygons.push_back(CSGPolygon<T>("{" + polygon1.getName() + "-" + polygon2.getName() + "}", mergedPolygonPoints));
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
			Vector2<double> lowestToNextPolygon1 = lowestPointPolygon1.vector(polygon1.getCwPoints()[nextPointIndexPolygon1]).template cast<double>();
			double verticalDot1 = Vector2<double>(0.0, 1.0).dotProduct(lowestToNextPolygon1.normalize());

			unsigned int nextPointIndexPolygon2 = (lowestPointIndexPolygon2+1)%polygon2.getCwPoints().size();
			Vector2<double> lowestToNextPolygon2 = lowestPointPolygon2.vector(polygon2.getCwPoints()[nextPointIndexPolygon2]).template cast<double>();
			double verticalDot2 = Vector2<double>(0.0, 1.0).dotProduct(lowestToNextPolygon2.normalize());

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

    template<class T> CSGIntersection<T> PolygonsUnion<T>::findFirstIntersectionOnEdge(const LineSegment2D<T> &edge, const Point2<T> &previousEdgePoint,
                                                                                            const CSGPolygon<T> *polygon, const CSGPolygon<T> *currentPolygon,
                                                                                            unsigned int currentPolygonEdgeId,
                                                                                            std::set<uint_fast64_t> &intersectionsProcessed) const
	{
		CSGIntersection<T> csgIntersection;
		csgIntersection.hasIntersection = false;

		T nearestSquareDistanceEdgeStartPoint = std::numeric_limits<T>::max();

		const std::vector<Point2<T>> &points = polygon->getCwPoints();
		for(unsigned int i=0, previousI=points.size()-1; i<points.size(); previousI=i++)
		{
            uint_fast64_t intersectionId = getIntersectionId(currentPolygon, currentPolygonEdgeId, polygon, previousI);
            auto itFind = intersectionsProcessed.find(intersectionId);
            if(itFind!=intersectionsProcessed.end())
            { //intersection already used in union of polygon.
                continue;
            }

			LineSegment2D<T> polygonEdge(points[previousI], points[i]);
			bool hasIntersection;
            Point2<T> intersectionPoint = edge.intersectPoint(polygonEdge, hasIntersection);

            if(hasIntersection)
            {
                bool ignoreThisIntersection = false;
                if(polygonEdge.getB()==intersectionPoint)
                {
                    //When intersection occurs on polygonEdge.B, the same intersection should occurs on nextPolygonEdge.A.
                    //Sometimes, it's not the case due to rounding error.
                    LineSegment2D<T> nextPolygonEdge(points[i], points[(i+1)%points.size()]);
                    edge.intersectPoint(nextPolygonEdge, ignoreThisIntersection);
                }

                T squareDistanceEdgeStartPoint = edge.getA().squareDistance(intersectionPoint);
                if (squareDistanceEdgeStartPoint < nearestSquareDistanceEdgeStartPoint && !ignoreThisIntersection)
                {
                    if ((edge.getA() != intersectionPoint && isIntersectionAngleBetter(edge, polygonEdge))
                        || (edge.getA() == intersectionPoint && isIntersectionAngleBetter(previousEdgePoint, edge, polygonEdge.getB())))
                    {
                        nearestSquareDistanceEdgeStartPoint = squareDistanceEdgeStartPoint;
                        csgIntersection.hasIntersection = true;
                        csgIntersection.intersectionPoint = intersectionPoint;
                        csgIntersection.intersectionId = intersectionId;
                        csgIntersection.edgeStartPointIndex = previousI;
                        csgIntersection.edgeEndPointIndex = i;
                    }
                }
            }
		}

		return csgIntersection;
	}

    template<class T> bool PolygonsUnion<T>::isIntersectionAngleBetter(const LineSegment2D<T> &edge, const LineSegment2D<T> &polygonEdge) const
	{
		Vector2<T> edgeVector = edge.toVector(); //identical but with less precision: edge.getA().vector(intersectionPoint);
		Vector2<T> polygonEdgeVector = polygonEdge.toVector(); //identical but with less precision: intersectionPoint.vector(polygonEdge.getB());
		T orientation = edgeVector.crossProduct(polygonEdgeVector);

		if(orientation > 0)
		{ //exterior angle is less than 180. Intersection point is a better candidate than edge.getB()
			return true;
		}else if(orientation==0)
		{ //angle is 180. check which edge is the most longest
			T lengthEdge = edgeVector.squareLength();
			T lengthIntersection = edge.getA().vector(polygonEdge.getB()).squareLength();

			return lengthIntersection > lengthEdge;
		}

		//exterior angle is greater than 180. edge.getB() point is a better candidate than intersectionPoint
		return false;
	}

    template<class T> bool PolygonsUnion<T>::isIntersectionAngleBetter(const Point2<T> &previousEdgePoint, const LineSegment2D<T> &edge, const Point2<T> &nextIntersectionPoint) const
    {
        #ifdef _DEBUG
        	assert(previousEdgePoint!=edge.getA());
        #endif

		Vector2<T> previousEdgeVector = LineSegment2D<T>(previousEdgePoint, edge.getA()).toVector();
		T nextEdgeOrientation = previousEdgeVector.crossProduct(edge.toVector());
		T intersectionEdgeOrientation = previousEdgeVector.crossProduct(edge.getA().vector(nextIntersectionPoint));

		if(nextEdgeOrientation>0 && intersectionEdgeOrientation<=0)
		{
			return false;
		}else if(nextEdgeOrientation<0 && intersectionEdgeOrientation>0)
		{
			return true;
		}

		Vector2<double> normalizedPreviousEdgeVector = previousEdgeVector.template cast<double>().normalize();
		double edgeAngle = normalizedPreviousEdgeVector.dotProduct(edge.toVector().template cast<double>().normalize());
		double intersectionEdgeAngle = normalizedPreviousEdgeVector.dotProduct(edge.getA().vector(nextIntersectionPoint).template cast<double>().normalize());

        if(edgeAngle==intersectionEdgeAngle)
        {
            return edge.getA().vector(nextIntersectionPoint).squareLength() > edge.toVector().squareLength();
        }else if(nextEdgeOrientation <= 0)
        {
            return intersectionEdgeAngle > edgeAngle;
        }

        return intersectionEdgeAngle < edgeAngle;
    }

    template<class T> uint_fast64_t PolygonsUnion<T>::getIntersectionId(const CSGPolygon<T> *polygon1, unsigned int edgeIdPolygon1,
                                                                        const CSGPolygon<T> *polygon2, unsigned int edgeIdPolygon2) const {
        int nameCompare = polygon1->getName().compare(polygon2->getName());

        #ifdef _DEBUG
            if(nameCompare==0)
            {
                throw std::runtime_error("Impossible to determine unique id for two polygons having same name: " + polygon1->getName());
            }
        #endif

        if(nameCompare>0)
        {
            std::swap(edgeIdPolygon1, edgeIdPolygon2);
        }

        uint_fast64_t intersectionId = static_cast<uint_fast64_t>(edgeIdPolygon1);
        intersectionId = intersectionId << 32;
        intersectionId += edgeIdPolygon2;
        return intersectionId;
    }

    template<class T> bool PolygonsUnion<T>::areSamePoints(const Point2<T> &p1, const Point2<T> &p2) const
	{
		constexpr T EPSILON = std::numeric_limits<T>::epsilon();
		return ((p1.X-EPSILON) <= p2.X) && ((p1.X+EPSILON) >= p2.X) && ((p1.Y-EPSILON) <= p2.Y) && ((p1.Y+EPSILON) >= p2.Y);
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
