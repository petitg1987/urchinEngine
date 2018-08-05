#include <algorithm>

#include "PolygonsSubtraction.h"

namespace urchin
{
    template<class T> IntersectionPoint<T>::IntersectionPoint(const Point2<T> &point, T squareDistanceToStartEdge) :
            point(point),
            squareDistanceToStartEdge(squareDistanceToStartEdge)
    {
    }

    template<class T> SubtractionPoint<T>::SubtractionPoint(const Point2<T> &point, bool isOutside, int crossPointIndex) :
            point(point),
            isOutside(isOutside),
            isProcessed(false),
            crossPointIndex(crossPointIndex)
    {
    }

    template<class T> SubtractionPoints<T>::SubtractionPoints(const std::vector<SubtractionPoint<T>> &minuend, const  std::vector<SubtractionPoint<T>> &subtrahend) :
            minuend(minuend),
            subtrahend(subtrahend)
    {
    }

    template<class T> std::vector<SubtractionPoint<T>> &SubtractionPoints<T>::operator [](SubtractionPoints<T>::PolygonType polygonType)
    {
        return (polygonType==SubtractionPoints<T>::MINUEND) ? minuend : subtrahend;
    }

    template<class T> std::vector<CSGPolygon<T>> PolygonsSubtraction<T>::subtractPolygons(const CSGPolygon<T> &minuendPolygon, const CSGPolygon<T> &subtrahendPolygon) const
    {
        bool subtrahendInside;
        return subtractPolygons(minuendPolygon, subtrahendPolygon, subtrahendInside);
    }

    /**
     * Perform a subtraction of polygons.
     * When subtrahendPolygon is totally included in minuendPolygon or touch edge: the minuendPolygon is returned without hole.
     * @param subtrahendInside True when subtrahendPolygon is totally included in minuendPolygon or touch edge.
     */
    template<class T> std::vector<CSGPolygon<T>> PolygonsSubtraction<T>::subtractPolygons(const CSGPolygon<T> &minuendPolygon, const CSGPolygon<T> &subtrahendPolygon, bool &subtrahendInside) const
    { //see http://www.pnnl.gov/main/publications/external/technical_reports/PNNL-SA-97135.pdf
        std::vector<CSGPolygon<T>> subtractedPolygons;

        SubtractionPoints<T> subtractionPoints = buildSubtractionPoints(minuendPolygon, subtrahendPolygon);
        #ifdef _DEBUG
//            logSubtractionPoints(minuendPolygon.getName(), subtractionPoints.minuend, subtrahendPolygon.getName(), subtractionPoints.subtrahend);
        #endif

        subtrahendInside = isSubtrahendInsideMinuend(subtractionPoints.subtrahend);
        if(subtrahendInside)
        {
            subtractedPolygons.emplace_back(minuendPolygon);
            return subtractedPolygons;
        }

        int startPointIndex;
        while((startPointIndex = findNextStartPointIndex(subtractionPoints.minuend))!=-1)
        {
            std::vector<Point2<T>> polygonCwPoints;
            polygonCwPoints.reserve(subtractionPoints.minuend.size()); //estimated memory size

            unsigned int maxIteration = static_cast<int>((minuendPolygon.getCwPoints().size() + subtrahendPolygon.getCwPoints().size()) + 1);
            unsigned int currentIteration = 0;

            typename SubtractionPoints<T>::PolygonType currentPolygon = SubtractionPoints<T>::MINUEND;
            int currentPointIndex = startPointIndex;
            while(currentIteration++ < maxIteration+1)
            {
                auto &currSubtractionPoint = subtractionPoints[currentPolygon][currentPointIndex];

                if(polygonCwPoints.empty() || polygonCwPoints[polygonCwPoints.size()-1]!=currSubtractionPoint.point)
                {
                    polygonCwPoints.emplace_back(currSubtractionPoint.point);
                }
                currSubtractionPoint.isProcessed = true;

                if(currSubtractionPoint.crossPointIndex!=-1)
                {
                    typename SubtractionPoints<T>::PolygonType otherPolygon = isMinuend(currentPolygon) ? SubtractionPoints<T>::SUBTRAHEND : SubtractionPoints<T>::MINUEND;;

                    int otherPointIndex = currSubtractionPoint.crossPointIndex;
                    int nextOtherPointOffset = computeNextPointOffset(otherPolygon, subtractionPoints);
                    int nextOtherPointIndex = (otherPointIndex + nextOtherPointOffset) % static_cast<int>(subtractionPoints[otherPolygon].size());

                    int nextPointOffset = computeNextPointOffset(currentPolygon, subtractionPoints);
                    int nextPointIndex = (currentPointIndex + nextPointOffset) % static_cast<int>(subtractionPoints[currentPolygon].size());

                    if( (isMinuend(currentPolygon) && !subtractionPoints[currentPolygon][nextPointIndex].isOutside && !subtractionPoints[otherPolygon][nextOtherPointIndex].isOutside)
                        || ((isSubtrahend(currentPolygon) && subtractionPoints[currentPolygon][nextPointIndex].isOutside) || subtractionPoints[otherPolygon][nextOtherPointIndex].isOutside) )
                    { //polygon switch
                        currentPointIndex = nextOtherPointIndex;
                        currentPolygon = otherPolygon;
                    }else
                    { //special case of collinear edge: don't perform polygon switch
                        currentPointIndex = nextPointIndex;
                    }
                }else
                {
                    int nextPointOffset = computeNextPointOffset(currentPolygon, subtractionPoints);
                    currentPointIndex = (currentPointIndex + nextPointOffset) % static_cast<int>(subtractionPoints[currentPolygon].size());
                }

                if(isMinuend(currentPolygon) && currentPointIndex==startPointIndex)
                {
                    break;
                }
            }

            if(currentIteration > maxIteration)
            {
                logInputData(minuendPolygon, subtrahendPolygon, "Maximum of iteration reached on polygons subtraction algorithm.", Logger::ERROR);
                subtractedPolygons.clear();
                return subtractedPolygons;
            }
            if(polygonCwPoints.size()<3)
            {
                logInputData(minuendPolygon, subtrahendPolygon, "Degenerate polygons built on polygons subtraction algorithm.", Logger::ERROR);
                subtractedPolygons.clear();
                return subtractedPolygons;
            }

            subtractedPolygons.push_back(CSGPolygon<T>("[" + minuendPolygon.getName() + "] - [" + subtrahendPolygon.getName()+ "]", polygonCwPoints));
        }

        return subtractedPolygons;
    }

    template<class T> SubtractionPoints<T> PolygonsSubtraction<T>::buildSubtractionPoints(const CSGPolygon<T> &minuendPolygon, const CSGPolygon<T> &subtrahendPolygon) const
    {
        std::map<unsigned int, std::vector<IntersectionPoint<T>>> minuendIntersections, subtrahendIntersections;
        buildIntersectionPoints(minuendPolygon, minuendIntersections, subtrahendPolygon, subtrahendIntersections);

        SubtractionPoints<T> subtractionPoints (buildSubtractionPoints(minuendPolygon, subtrahendPolygon, minuendIntersections),
                                                buildSubtractionPoints(subtrahendPolygon, minuendPolygon, subtrahendIntersections));

        computeCrossPointIndex(subtractionPoints.minuend, subtractionPoints.subtrahend);

        return subtractionPoints;
    }

    template<class T> void PolygonsSubtraction<T>::buildIntersectionPoints(const CSGPolygon<T> &minuendPolygon, std::map<unsigned int, std::vector<IntersectionPoint<T>>> &minuendIntersections,
                                                                           const CSGPolygon<T> &subtrahendPolygon, std::map<unsigned int, std::vector<IntersectionPoint<T>>> &subtrahendIntersections) const
    {
        for(unsigned int i=0; i<minuendPolygon.getCwPoints().size(); ++i)
        {
            size_t nextI = (i+1)%minuendPolygon.getCwPoints().size();
            LineSegment2D<T> minuendEdge(minuendPolygon.getCwPoints()[i], minuendPolygon.getCwPoints()[nextI]);
            LineSegment2D<double> minuendEdgeDouble = minuendEdge.template cast<double>();

            for(unsigned int j=0; j<subtrahendPolygon.getCwPoints().size(); ++j)
            {
                size_t nextJ = (j+1)%subtrahendPolygon.getCwPoints().size();
                LineSegment2D<T> subtrahendEdge(subtrahendPolygon.getCwPoints()[j], subtrahendPolygon.getCwPoints()[nextJ]);
                LineSegment2D<double> subtrahendEdgeDouble = subtrahendEdge.template cast<double>();

                bool hasIntersection, hasFarthestIntersection;
                Point2<double> farthestIntersectionPoint;
                Point2<double> intersectionPoint = minuendEdgeDouble.intersectPoint(subtrahendEdgeDouble, hasIntersection, farthestIntersectionPoint, hasFarthestIntersection);

                if(hasIntersection)
                {
                    pushIntersectionPoint(minuendEdge, subtrahendEdge, intersectionPoint.template cast<T>(), minuendIntersections[i]);
                    pushIntersectionPoint(subtrahendEdge, minuendEdge, intersectionPoint.template cast<T>(), subtrahendIntersections[j]);

                    if(hasFarthestIntersection)
                    {
                        pushIntersectionPoint(minuendEdge, subtrahendEdge, farthestIntersectionPoint.template cast<T>(), minuendIntersections[i]);
                        pushIntersectionPoint(subtrahendEdge, minuendEdge, farthestIntersectionPoint.template cast<T>(), subtrahendIntersections[j]);
                    }
                }
            }

            std::sort(minuendIntersections[i].begin(), minuendIntersections[i].end(), [&](const IntersectionPoint<T> &left, const IntersectionPoint<T> &right)
                {return left.squareDistanceToStartEdge < right.squareDistanceToStartEdge;});
        }

        for (auto& intersectionPoints : subtrahendIntersections)
        {
            std::sort(intersectionPoints.second.begin(), intersectionPoints.second.end(), [&](const IntersectionPoint<T> &left, const IntersectionPoint<T> &right)
                    {return left.squareDistanceToStartEdge < right.squareDistanceToStartEdge;});
        }
    }

    template<class T> void PolygonsSubtraction<T>::pushIntersectionPoint(const LineSegment2D<T> &edge, const LineSegment2D<T> &otherEdge, const Point2<T> &intersectionPoint,
                                                                         std::vector<IntersectionPoint<T>> &intersectionPointsVector) const
    {
        T distanceEdgeA = edge.getA().squareDistance(intersectionPoint);
        if(edge.getB().squareDistance(intersectionPoint) != 0) //avoid duplicate intersection point (same intersection in nextEdge(edge).getA())
        {
            intersectionPointsVector.emplace_back(IntersectionPoint<T>(intersectionPoint, distanceEdgeA));
        }
    }

    template<class T> std::vector<SubtractionPoint<T>> PolygonsSubtraction<T>::buildSubtractionPoints(const CSGPolygon<T> &polygon, const CSGPolygon<T> &otherPolygon,
                                                                                                      const std::map<unsigned int, std::vector<IntersectionPoint<T>>> &intersections) const
    {
        std::vector<SubtractionPoint<T>> subtractionPoints;
        subtractionPoints.reserve(polygon.getCwPoints().size() + intersections.size());

        for(unsigned int i=0; i<polygon.getCwPoints().size(); ++i)
        {
            const Point2<T> &point = polygon.getCwPoints()[i];
            bool isOutside = !otherPolygon.pointInsideOrOnPolygon(point);
            if(subtractionPoints.empty() || subtractionPoints[subtractionPoints.size()-1].point!=point)
            {
                subtractionPoints.push_back(SubtractionPoint<T>(point, isOutside, -1));
            }

            const auto &intersectionsPointsIt = intersections.find(i);
            if(intersectionsPointsIt==intersections.end())
            {
                continue;
            }

            const std::vector<IntersectionPoint<T>> &intersectionsPoints = intersectionsPointsIt->second;
            for (unsigned int intersectionI = 0; intersectionI < intersectionsPoints.size(); ++intersectionI)
            {
                IntersectionPoint<T> intersectionPoint = intersectionsPoints[intersectionI];

                if (intersectionI!=0)
                {
                    Point2<T> middlePoint = determineMiddlePoint(intersectionsPoints[intersectionI - 1].point, intersectionPoint.point);
                    bool isMiddlePointOutside = !otherPolygon.pointInsideOrOnPolygon(middlePoint);
                    if(isMiddlePointOutside)
                    {
                        subtractionPoints.emplace_back(SubtractionPoint<T>(middlePoint, true, -1));
                    }
                }

                constexpr int UNDEFINED_CROSS_POINT = -2;
                subtractionPoints.emplace_back(SubtractionPoint<T>(intersectionPoint.point, false, UNDEFINED_CROSS_POINT));
            }
        }

        return subtractionPoints;
    }

    template<class T> Point2<T> PolygonsSubtraction<T>::determineMiddlePoint(const Point2<T> &point1, const Point2<T> &point2) const
    {
        Point2<T> summedPoint = point1 + point2;
        return Point2<T>(summedPoint.X/2.0, summedPoint.Y/2.0);
    }

    template<class T> void PolygonsSubtraction<T>::computeCrossPointIndex(std::vector<SubtractionPoint<T>> &minuendPoints, std::vector<SubtractionPoint<T>> &subtrahendPoints) const
    {
        for(unsigned int i=0; i<minuendPoints.size(); ++i)
        {
            for(unsigned int j=0; j<subtrahendPoints.size(); ++j)
            {
                if(minuendPoints[i].point==subtrahendPoints[j].point && minuendPoints[i].crossPointIndex < 0 && subtrahendPoints[j].crossPointIndex < 0)
                {
                    minuendPoints[i].crossPointIndex = j;
                    minuendPoints[i].isOutside = false; //because of rounding errors

                    subtrahendPoints[j].crossPointIndex = i;
                    subtrahendPoints[j].isOutside = false; //because of rounding errors
                }
            }
        }
    }

    template<class T> bool PolygonsSubtraction<T>::isSubtrahendInsideMinuend(const std::vector<SubtractionPoint<T>> &subtrahendPoints) const
    {
        for(const auto &subtrahendPoint : subtrahendPoints)
        {
            if(subtrahendPoint.isOutside)
            {
                return false;
            }
        }
        return true;
    }

    template<class T> int PolygonsSubtraction<T>::findNextStartPointIndex(const std::vector<SubtractionPoint<T>> &subtractionPoint) const
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

    template<class T> int PolygonsSubtraction<T>::computeNextPointOffset(typename SubtractionPoints<T>::PolygonType polygonType, const SubtractionPoints<T> &subtractionPoints) const
    {
        if(SubtractionPoints<T>::MINUEND==polygonType)
        {
            return 1; //CW
        }

        return static_cast<int>(subtractionPoints.subtrahend.size()) - 1; //CCW
    }

    template<class T> bool PolygonsSubtraction<T>::isMinuend(typename SubtractionPoints<T>::PolygonType polygonType) const
    {
        return SubtractionPoints<T>::MINUEND==polygonType;
    }

    template<class T> bool PolygonsSubtraction<T>::isSubtrahend(typename SubtractionPoints<T>::PolygonType polygonType) const
    {
        return SubtractionPoints<T>::SUBTRAHEND==polygonType;
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

    template<class T> void PolygonsSubtraction<T>::logInputData(const CSGPolygon<T> &minuendPolygon, const CSGPolygon<T> &subtrahendPolygon, const std::string &message,
                                                                Logger::CriticalityLevel logLevel) const
    {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<T>::max_digits10);

        logStream<<message<<std::endl;
        logStream<<" - Minuend polygon: "<<std::endl<<minuendPolygon<<std::endl;
        logStream<<" - Subtrahend Polygon: "<<std::endl<<subtrahendPolygon;

        Logger::logger().log(logLevel, logStream.str());
    }

    //explicit template
    template class SubtractionPoint<float>;

    template class PolygonsSubtraction<float>;
}
