#include <algorithm>

#include "PolygonsSubtraction.h"

namespace urchin
{
    template<class T> SubtractionPoint<T>::SubtractionPoint(const Point2<T> &point, bool isOutside, int crossPointIndex) :
            point(point),
            isOutside(isOutside),
            isProcessed(false),
            crossPointIndex(crossPointIndex)
    {
    }

    template<class T> IntersectionPoint<T>::IntersectionPoint(const Point2<T> &point, unsigned int squareDistanceToStartEdge) :
            point(point),
            squareDistanceToStartEdge(squareDistanceToStartEdge)
    {
    }

    template<class T> PolygonsSubtraction<T>::PolygonsSubtraction()
    {

    }

    template<class T> PolygonsSubtraction<T>::~PolygonsSubtraction()
    {

    }

    /**
     * Perform a subtraction of polygons.
     * In case of subtrahendPolygon is totally included in minuendPolygon: the minuendPolygon is returned without hole inside.
     */
    template<class T> std::vector<CSGPolygon<T>> PolygonsSubtraction<T>::subtractPolygons(const CSGPolygon<T> &minuendPolygon, const CSGPolygon<T> &subtrahendPolygon) const
    { //see http://www.pnnl.gov/main/publications/external/technical_reports/PNNL-SA-97135.pdf
        std::vector<CSGPolygon<T>> subtractedPolygons;

        std::map<unsigned int, std::vector<IntersectionPoint<T>>> minuendIntersectionsPolygon, subtrahendIntersectionsPolygon;
        buildIntersectionPoints(minuendPolygon, minuendIntersectionsPolygon, subtrahendPolygon, subtrahendIntersectionsPolygon);

        std::vector<SubtractionPoint<T>> minuendPoints = buildSubtractionPoints(minuendPolygon, subtrahendPolygon, minuendIntersectionsPolygon, subtrahendIntersectionsPolygon);
        std::vector<SubtractionPoint<T>> subtrahendPoints = buildSubtractionPoints(subtrahendPolygon, minuendPolygon, subtrahendIntersectionsPolygon, minuendIntersectionsPolygon);
        if(minuendPoints.size()==0 || subtrahendPoints.size()==0)
        {
            return subtractedPolygons;
        }

        #ifdef _DEBUG
            //logSubtractionPoints(minuendPolygon.getName(), minuendPoints, subtrahendPolygon.getName(), subtrahendPoints);
        #endif

        int startPointIndex;
        while((startPointIndex = findNextPointIndex(minuendPoints))!=-1)
        {
            std::vector<Point2<T>> polygonCwPoints;
            polygonCwPoints.reserve(minuendPoints.size()); //estimated memory size

            unsigned int maxIteration = (minuendPolygon.getCwPoints().size() + subtrahendPolygon.getCwPoints().size());
            unsigned int currentIteration = 0;

            bool onMinuendPolygon = true;
            int currentPointIndex = startPointIndex;
            while(currentIteration++ < maxIteration+1)
            {
                if(onMinuendPolygon) //TODO handle same edge
                {
                    polygonCwPoints.push_back(minuendPoints[currentPointIndex].point);
                    minuendPoints[currentPointIndex].isProcessed = true;

                    if(minuendPoints[currentPointIndex].crossPointIndex!=-1)
                    {
                        onMinuendPolygon = false;
                        currentPointIndex = (minuendPoints[currentPointIndex].crossPointIndex + subtrahendPoints.size() - 1) % subtrahendPoints.size();
                    }else
                    {
                        currentPointIndex = (currentPointIndex + 1) % minuendPoints.size();
                    }
                }else
                {
                    polygonCwPoints.push_back(subtrahendPoints[currentPointIndex].point);

                    if(subtrahendPoints[currentPointIndex].crossPointIndex!=-1)
                    {
                        onMinuendPolygon = true;
                        currentPointIndex = (subtrahendPoints[currentPointIndex].crossPointIndex + 1) % minuendPoints.size();
                    }else
                    {
                        currentPointIndex = (currentPointIndex + subtrahendPoints.size() - 1) % subtrahendPoints.size();
                    }
                }

                if(onMinuendPolygon)
                {

                }

                if( (onMinuendPolygon && currentPointIndex==startPointIndex)
                    || (!onMinuendPolygon && subtrahendPoints[currentPointIndex].crossPointIndex==startPointIndex))
                {
                    break;
                }
            }

            if(currentIteration > maxIteration)
            {
                logInputData(minuendPolygon, subtrahendPolygon, "Maximum of iteration reached on polygons subtraction algorithm.", Logger::ERROR);
                break;
            }else
            {
                subtractedPolygons.push_back(CSGPolygon<T>("(" + minuendPolygon.getName() + "-subBy-" + subtrahendPolygon.getName()+ ")", polygonCwPoints));
            }
        }

        return subtractedPolygons;
    }

    template<class T> void PolygonsSubtraction<T>::buildIntersectionPoints(const CSGPolygon<T> &polygon1, std::map<unsigned int, std::vector<IntersectionPoint<T>>> &intersectionsPolygon1,
                                                                           const CSGPolygon<T> &polygon2, std::map<unsigned int, std::vector<IntersectionPoint<T>>> &intersectionsPolygon2) const
    {
        for(unsigned int i=0; i<polygon1.getCwPoints().size(); ++i)
        {
            unsigned int nextI = (i+1)%polygon1.getCwPoints().size();
            LineSegment2D<T> edge1(polygon1.getCwPoints()[i], polygon1.getCwPoints()[nextI]);

            for(unsigned int j=0; j<polygon2.getCwPoints().size(); ++j)
            {
                unsigned int nextJ = (j+1)%polygon2.getCwPoints().size();
                LineSegment2D<T> edge2(polygon2.getCwPoints()[j], polygon2.getCwPoints()[nextJ]);

                bool hasIntersection, hasFarthestIntersection;
                Point2<T> farthestIntersectionPoint;
                Point2<T> intersectionPoint = edge1.intersectPoint(edge2, hasIntersection, farthestIntersectionPoint, hasFarthestIntersection);
                if(hasIntersection)
                {
                    intersectionsPolygon1[i].push_back(IntersectionPoint<T>(intersectionPoint, edge1.getA().squareDistance(intersectionPoint)));
                    intersectionsPolygon2[j].push_back(IntersectionPoint<T>(intersectionPoint, edge2.getA().squareDistance(intersectionPoint)));

                    if(hasFarthestIntersection)
                    {
                        intersectionsPolygon1[i].push_back(IntersectionPoint<T>(farthestIntersectionPoint, edge1.getA().squareDistance(farthestIntersectionPoint)));
                        intersectionsPolygon2[j].push_back(IntersectionPoint<T>(farthestIntersectionPoint, edge2.getA().squareDistance(farthestIntersectionPoint)));
                    }
                }
            }

            std::sort(intersectionsPolygon1[i].begin(), intersectionsPolygon1[i].end(), [&](const IntersectionPoint<T> &left, const IntersectionPoint<T> &right)
                {return left.squareDistanceToStartEdge < right.squareDistanceToStartEdge;});
        }

        for (auto& intersectionPoints : intersectionsPolygon2)
        {
            std::sort(intersectionPoints.second.begin(), intersectionPoints.second.end(), [&](const IntersectionPoint<T> &left, const IntersectionPoint<T> &right)
                    {return left.squareDistanceToStartEdge < right.squareDistanceToStartEdge;});
        }
    }

    template<class T> std::vector<SubtractionPoint<T>> PolygonsSubtraction<T>::buildSubtractionPoints(const CSGPolygon<T> &polygon, const CSGPolygon<T> &otherPolygon,
                                                                                                      const std::map<unsigned int, std::vector<IntersectionPoint<T>>> &intersections,
                                                                                                      const std::map<unsigned int, std::vector<IntersectionPoint<T>>> &otherIntersections) const
    {
        std::vector<SubtractionPoint<T>> subtractionPoints;
        subtractionPoints.reserve(polygon.getCwPoints().size() + intersections.size());

        for(unsigned int i=0; i<polygon.getCwPoints().size(); ++i)
        {
            const Point2<T> &point = polygon.getCwPoints()[i];
            bool isOutside = !otherPolygon.pointInsidePolygon(point);
            subtractionPoints.push_back(SubtractionPoint<T>(point, isOutside, -1));

            const auto &intersectionsPointsIt = intersections.find(i);
            if(intersectionsPointsIt!=intersections.end())
            {
                const std::vector<IntersectionPoint<T>> &intersectionsPoints = intersectionsPointsIt->second;
                for (unsigned int intersectionI = 0; intersectionI < intersectionsPoints.size(); ++intersectionI)
                {
                    IntersectionPoint<T> intersectionPoint = intersectionsPoints[intersectionI];
                    int crossPointIndex = computeCrossPointIndex(intersectionPoint.point, otherIntersections);
                    if (crossPointIndex == -1)
                    {
                        logInputData(polygon, otherPolygon, "Impossible to find cross point index on intersection point.", Logger::ERROR);
                        std::vector<SubtractionPoint<T>> emptySubtractionPoints;
                        return emptySubtractionPoints;
                    }

                    subtractionPoints.push_back(SubtractionPoint<T>(intersectionPoint.point, false, crossPointIndex));

                    if (intersectionI % 2 == 1 && intersectionI != intersectionsPoints.size() - 1)
                    {
                        Point2<T> middlePoint = determineMiddlePoint(intersectionPoint.point, intersectionsPoints[intersectionI + 1].point);
                        subtractionPoints.push_back(SubtractionPoint<T>(middlePoint, true, -1));
                    }
                }
            }
        }

        return subtractionPoints;
    }

    template<class T> int PolygonsSubtraction<T>::computeCrossPointIndex(const Point2<T> &intersectionPoint,
                                                                         const std::map<unsigned int, std::vector<IntersectionPoint<T>>> &otherIntersections) const
    {
        unsigned int otherIntersectionPointsCount = 0;
        for(const auto &otherIntersection : otherIntersections)
        {
            unsigned int edgeIntersectionPointsCount = 0;
            for(IntersectionPoint<T> otherIntersectionPoint : otherIntersection.second)
            {
                if(otherIntersectionPoint.point==intersectionPoint)
                {
                    return otherIntersection.first + otherIntersectionPointsCount + edgeIntersectionPointsCount + 1;
                }else if(edgeIntersectionPointsCount%2==1 && edgeIntersectionPointsCount!=otherIntersection.second.size()-1)
                { //for middle point
                    edgeIntersectionPointsCount+=2;
                }else
                {
                    edgeIntersectionPointsCount++;
                }
            }
            otherIntersectionPointsCount+=edgeIntersectionPointsCount;
        }

        return -1;
    }

    template<class T> Point2<T> PolygonsSubtraction<T>::determineMiddlePoint(const Point2<T> &point1, const Point2<T> &point2) const
    {
        Point2<T> summedPoint = point1 + point2;
        if(typeid(int)==typeid(T) || typeid(long)==typeid(T) || typeid(long long)==typeid(T))
        {
            return Point2<T>(MathAlgorithm::roundDivision<T>(summedPoint.X, 2),
                             MathAlgorithm::roundDivision<T>(summedPoint.Y, 2));
        }

        return Point2<T>(summedPoint.X/2.0, summedPoint.Y/2.0);
    }

    template<class T> int PolygonsSubtraction<T>::findNextPointIndex(const std::vector<SubtractionPoint<T>> &subtractionPoint) const
    {
        for(unsigned int i=0; i<subtractionPoint.size(); ++i)
        {
            if(!subtractionPoint[i].isProcessed && subtractionPoint[i].isOutside)
            {
                return i;
            }
        }
        return -1;
    }

    template<class T> void PolygonsSubtraction<T>::logSubtractionPoints(const std::string &minuendName, const std::vector<SubtractionPoint<T>> &minuendPoints,
                                                                        const std::string &subtrahendName, const std::vector<SubtractionPoint<T>> &subtrahendPoints) const
    {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<T>::max_digits10);

        logStream<<"Subtraction points computed from polygon subtraction algorithm:"<<std::endl;
        logStream<<minuendName<<":"<<std::endl;
        unsigned int i=0;
        for(SubtractionPoint<T> p : minuendPoints)
        {
            logStream<<std::setw(5)<<i++<<")"<<std::setw(5)<<p.isOutside <<std::setw(5)<<p.crossPointIndex<<std::setw(15)<<p.point<<std::endl;
        }

        logStream<<subtrahendName<<":"<<std::endl;
        unsigned int j=0;
        for(SubtractionPoint<T> p : subtrahendPoints)
        {
            logStream<<std::setw(5)<<j++<<")"<<std::setw(5)<<p.isOutside <<std::setw(5)<<p.crossPointIndex<<std::setw(15)<<p.point<<std::endl;
        }

        Logger::logger().logInfo(logStream.str());
    }

    template<class T> void PolygonsSubtraction<T>::logInputData(const CSGPolygon<T> &polygon1, const CSGPolygon<T> &polygon2, const std::string &message, Logger::CriticalityLevel logLevel) const
    {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<T>::max_digits10);

        logStream<<message<<std::endl;
        logStream<<" - Polygon 1: "<<std::endl<<polygon1<<std::endl;
        logStream<<" - Polygon 2: "<<std::endl<<polygon2;

        Logger::logger().log(logLevel, logStream.str());
    }

    //explicit template
    template class SubtractionPoint<float>;
    template class SubtractionPoint<double>;
    template class SubtractionPoint<int>;
    template class SubtractionPoint<long long>;

    template class IntersectionPoint<float>;
    template class IntersectionPoint<double>;
    template class IntersectionPoint<int>;
    template class IntersectionPoint<long long>;

    template class PolygonsSubtraction<float>;
    template class PolygonsSubtraction<double>;
    template class PolygonsSubtraction<int>;
    template class PolygonsSubtraction<long long>;
}
