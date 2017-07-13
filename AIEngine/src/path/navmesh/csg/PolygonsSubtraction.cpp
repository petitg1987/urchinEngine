#include <algorithm>

#include "PolygonsSubtraction.h"

namespace urchin
{
    template<class T> IntersectionPoint<T>::IntersectionPoint(const Point2<T> &intersectionPoint, int crossPointIndex) :
            intersectionPoint(intersectionPoint),
            crossPointIndex(crossPointIndex)
    {

    }

    template<class T> PolygonsSubtraction<T>::PolygonsSubtraction()
    {

    }

    template<class T> PolygonsSubtraction<T>::~PolygonsSubtraction()
    {

    }

    template<class T> std::vector<CSGPolygon<T>> PolygonsSubtraction<T>::subtractPolygons(const CSGPolygon<T> &minuendPolygon, const CSGPolygon<T> &subtrahendPolygon) const
    { //see http://www.pnnl.gov/main/publications/external/technical_reports/PNNL-SA-97135.pdf
        std::map<unsigned int, std::vector<SubtractionPoint>> minuendPoints1, subtrahendPoints2;
        buildSubtractionPointsFor(minuendPolygon, minuendPoints1, subtrahendPolygon, subtrahendPoints2);
    }

    template<class T> void PolygonsSubtraction<T>::buildSubtractionPointsFor(const CSGPolygon<T> &polygon1, std::map<unsigned int, std::vector<SubtractionPoint>> &subtractionPoints1,
                                                                             const CSGPolygon<T> &polygon2, std::map<unsigned int, std::vector<SubtractionPoint>> &subtractionPoints2) const
    {
        std::map<unsigned int, std::vector<IntersectionPoint>> intersectionsPolygon1, intersectionsPolygon2;
        buildIntersectionPoints(polygon1, intersectionsPolygon1, polygon2, intersectionsPolygon2);


                /*
        std::vector<SubtractionPoint<T>> result;
        result.reserve(minuendPolygon.getCwPoints().size() + minuendPolygon.getCwPoints().size()/3); //estimated memory size
        for(unsigned int i=0, previousI=minuendPolygon.getCwPoints().size()-1; i<minuendPolygon.getCwPoints().size(); previousI=i++)
        {
            SubtractionPoint<T> subtractionPoint;
            subtractionPoint.isOutside = !pointInsidePolygon(subtrahendPolygon, minuendPolygon.getCwPoints()[i]);
            subtractionPoint.isProcessed = false;
            subtractionPoint.crossPointIndex = -1;
            result.push_back(subtractionPoint);

            LineSegment2D<T> edge(minuendPolygon.getCwPoints()[previousI], minuendPolygon.getCwPoints()[i]);
            std::vector<Point2<T>> intersectionsPoints = findIntersectionPoints(edge, subtrahendPolygon);
            //add extra points between groups of 2 so there will be a distinct point outside
            for (unsigned int j=intersectionsPoints.size()-2; j>0; j-=2)
            {
                Point2<T> middlePoint = determineMiddlePoint(intersectionsPoints[j-1], intersectionsPoints[j]);
                //TODO add mid point in intersectionsPoints: cross=-1; process=false, isOutside=true
            }


            for(Point2<T> intersectionPoint : intersectionsPoints)
            {
                //TODO add intersections points
            }
        } */
    }

    template<class T> void PolygonsSubtraction<T>::buildIntersectionPoints(const CSGPolygon<T> &polygon1, std::map<unsigned int, std::vector<IntersectionPoint>> &intersectionsPolygon1,
                                                                             const CSGPolygon<T> &polygon2, std::map<unsigned int, std::vector<IntersectionPoint>> &intersectionsPolygon2) const
    {
        for(unsigned int i=0, previousI=polygon1.getCwPoints().size()-1; i<polygon1.getCwPoints().size(); previousI=i++)
        {
            LineSegment2D<T> edge1(polygon1.getCwPoints()[previousI], polygon1.getCwPoints()[i]);
            for(unsigned int j=0, previousJ=polygon2.getCwPoints().size()-1; j<polygon2.getCwPoints().size(); previousJ=j++)
            {
                LineSegment2D<T> edge2(polygon2.getCwPoints()[previousJ], polygon2.getCwPoints()[j]);

                bool endPointUsed = false;
                if(edge1.squareDistance(edge2.getA())==0)
                {
                    intersectionsPolygon1[i].push_back(IntersectionPoint(edge2.getA(), j));
                    endPointUsed = true;
                }
                if(edge1.squareDistance(edge2.getB())==0)
                {
                    intersectionsPolygon1[i].push_back(IntersectionPoint(edge2.getB(), j));
                    endPointUsed = true;
                }
                if(edge2.squareDistance(edge1.getA())==0)
                {
                    intersectionsPolygon2[j].push_back(IntersectionPoint(edge1.getA(), i));
                    endPointUsed = true;
                }
                if(edge2.squareDistance(edge1.getB())==0)
                {
                    intersectionsPolygon2[j].push_back(IntersectionPoint(edge1.getB(), i));
                    endPointUsed = true;
                }

                if(!endPointUsed)
                {
                    bool hasIntersection;
                    Point2<T> intersectionPoint = edge1.intersectPoint(edge2, hasIntersection);
                    if(hasIntersection)
                    {
                        intersectionsPolygon1[i].push_back(IntersectionPoint(intersectionPoint, j));
                        intersectionsPolygon2[j].push_back(IntersectionPoint(intersectionPoint, i));
                    }
                }
            }
        }

        for (const auto& intersectionPoints : intersectionsPolygon1)
        {
            std::sort(intersectionPoints.second.begin(), intersectionPoints.second.end(), [&](const Point2<T> &left, const Point2<T> &right) {
                    Point2<T> edgeA = polygon1.getCwPoints()[intersectionPoints.first];
                    return left.squareDistance(edgeA) > right.squareDistance(edgeA);});
        }

        for (const auto& intersectionPoints : intersectionsPolygon2)
        {
            std::sort(intersectionPoints.second.begin(), intersectionPoints.second.end(), [&](const Point2<T> &left, const Point2<T> &right) {
                Point2<T> edgeA = polygon2.getCwPoints()[intersectionPoints.first];
                return left.squareDistance(edgeA) > right.squareDistance(edgeA);});
        }
    }

    template<class T> bool PolygonsSubtraction<T>::pointInsidePolygon(const CSGPolygon<T> &polygon, const Point2<T> &point) const //TODO almost duplicate with PolygonsUnion.
    { //see http://web.archive.org/web/20120323102807/http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
        bool inside = false;

        const std::vector<Point2<T>> &points = polygon.getCwPoints();
        for(unsigned int i=0, previousI=points.size()-1; i<points.size(); previousI=i++)
        {
            Point2<T> point1 = points[previousI];
            Point2<T> point2 = points[i];

            if (((point1.Y<=point.Y && point.Y<point2.Y) || (point2.Y<=point.Y && point.Y<point1.Y))
                //same but without division: ((point.X-point1.X) < (point2.X-point1.X) * (point.Y-point1.Y) / (point2.Y-point1.Y))
                && ((point.X-point1.X)*std::abs(point2.Y-point1.Y) < (point2.X-point1.X) * (point.Y-point1.Y) * MathAlgorithm::sign<T>(point2.Y-point1.Y)))
            {
                inside = !inside;
            }
        }

        return inside;
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

    //explicit template
    template class PolygonsSubtraction<float>;

    template class PolygonsSubtraction<double>;

    template class PolygonsSubtraction<int>;

    template class PolygonsSubtraction<long long>;

}
