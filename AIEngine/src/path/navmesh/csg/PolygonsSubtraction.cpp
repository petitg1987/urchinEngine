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

    template<class T> IntersectionPoint<T>::IntersectionPoint(const Point2<T> &point, int crossPointIndex, unsigned int squareDistStartEdge) :
            point(point),
            crossPointIndex(crossPointIndex),
            squareDistStartEdge(squareDistStartEdge)
    {
    }

    template<class T> bool IntersectionPoint<T>::operator()(IntersectionPoint<T> left, IntersectionPoint<T> right) const
    {
        return left.squareDistStartEdge > right.squareDistStartEdge;
    }

    template<class T> PolygonsSubtraction<T>::PolygonsSubtraction()
    {

    }

    template<class T> PolygonsSubtraction<T>::~PolygonsSubtraction()
    {

    }

    template<class T> std::vector<CSGPolygon<T>> PolygonsSubtraction<T>::subtractPolygons(const CSGPolygon<T> &minuendPolygon, const CSGPolygon<T> &subtrahendPolygon) const
    { //see http://www.pnnl.gov/main/publications/external/technical_reports/PNNL-SA-97135.pdf
        std::vector<CSGPolygon<T>> result;

        std::map<unsigned int, std::vector<IntersectionPoint<T>>> minuendIntersectionsPolygon, subtrahendIntersectionsPolygon;
        buildIntersectionPoints(minuendPolygon, minuendIntersectionsPolygon, subtrahendPolygon, subtrahendIntersectionsPolygon);

        std::vector<SubtractionPoint<T>> minuendPoints = buildSubtractionPoints(minuendPolygon, subtrahendPolygon, minuendIntersectionsPolygon);
        std::vector<SubtractionPoint<T>> subtrahendPoints = buildSubtractionPoints(subtrahendPolygon, minuendPolygon, subtrahendIntersectionsPolygon);

        //TODO implement algo loop

        return result;
    }

    template<class T> void PolygonsSubtraction<T>::buildIntersectionPoints(const CSGPolygon<T> &polygon1, std::map<unsigned int, std::vector<IntersectionPoint<T>>> &intersectionsPolygon1,
                                                                           const CSGPolygon<T> &polygon2, std::map<unsigned int, std::vector<IntersectionPoint<T>>> &intersectionsPolygon2) const
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
                    intersectionsPolygon1[i].push_back(IntersectionPoint<T>(edge2.getA(), j, edge1.getA().squareDistance(edge2.getA())));
                    endPointUsed = true;
                }
                if(edge1.squareDistance(edge2.getB())==0)
                {
                    intersectionsPolygon1[i].push_back(IntersectionPoint<T>(edge2.getB(), j, edge1.getA().squareDistance(edge2.getB())));
                    endPointUsed = true;
                }
                if(edge2.squareDistance(edge1.getA())==0)
                {
                    intersectionsPolygon2[j].push_back(IntersectionPoint<T>(edge1.getA(), i, edge2.getA().squareDistance(edge1.getA())));
                    endPointUsed = true;
                }
                if(edge2.squareDistance(edge1.getB())==0)
                {
                    intersectionsPolygon2[j].push_back(IntersectionPoint<T>(edge1.getB(), i, edge2.getA().squareDistance(edge1.getB())));
                    endPointUsed = true;
                }

                if(!endPointUsed)
                {
                    bool hasIntersection;
                    Point2<T> intersectionPoint = edge1.intersectPoint(edge2, hasIntersection);
                    if(hasIntersection)
                    {
                        intersectionsPolygon1[i].push_back(IntersectionPoint<T>(intersectionPoint, j, edge1.getA().squareDistance(intersectionPoint)));
                        intersectionsPolygon2[j].push_back(IntersectionPoint<T>(intersectionPoint, i, edge2.getA().squareDistance(intersectionPoint)));
                    }
                }
            }
        }

        for (const auto& intersectionPoints : intersectionsPolygon1)
        {
            std::sort(intersectionPoints.second.begin(), intersectionPoints.second.end(), intersectionPoints);
        }

        for (const auto& intersectionPoints : intersectionsPolygon2)
        {
            //TODO std::sort(intersectionPoints.second.begin(), intersectionPoints.second.end(), intersectionPoints);
        }
    }

    template<class T> std::vector<SubtractionPoint<T>> PolygonsSubtraction<T>::buildSubtractionPoints(const CSGPolygon<T> &polygon, const CSGPolygon<T> &otherPolygon,
                                                                                                      std::map<unsigned int, std::vector<IntersectionPoint<T>>> intersectionsPolygon) const
    {
        std::vector<SubtractionPoint<T>> subtractionPoints;
        subtractionPoints.reserve(polygon.getCwPoints().size() + intersectionsPolygon.size());

        for(unsigned int i=0; i<polygon.getCwPoints().size(); ++i)
        {
            bool isOutside = !pointInsidePolygon(otherPolygon, polygon.getCwPoints()[i]);
            subtractionPoints.push_back(SubtractionPoint<T>(polygon.getCwPoints()[i], isOutside, -1));

            std::vector<IntersectionPoint<T>> intersectionsPoints = intersectionsPolygon[i];
            for(unsigned int intersectionI=0; intersectionI<intersectionsPoints.size(); ++i)
            {
                IntersectionPoint<T> intersectionPoint = intersectionsPoints[intersectionI];
                subtractionPoints.push_back(SubtractionPoint<T>(intersectionPoint.point, false, intersectionPoint.crossPointIndex));

                if(intersectionI%2==1 && intersectionI!=intersectionsPoints.size()-1)
                {
                    Point2<T> middlePoint = determineMiddlePoint(intersectionsPoints[intersectionI - 1].point, intersectionPoint.point);
                    subtractionPoints.push_back(SubtractionPoint<T>(middlePoint, true, -1));
                }
            }
        }

        return subtractionPoints;
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
