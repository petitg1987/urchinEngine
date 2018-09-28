#include <algorithm>

#include "HeightfieldPointHelper.h"
#include "math/algorithm/MathAlgorithm.h"

#define PARALLEL_EPSILON 0.07 //= 4 degrees (asin(4/180*pi))

namespace urchin
{

    template <class T> DistanceToStartPointComp<T>::DistanceToStartPointComp(const Point3<T> &startPoint) :
            startPoint(startPoint)
    {
    }

    template <class T> bool DistanceToStartPointComp<T>::operator() (const Point3<T> &left, const Point3<T> &right) const
    {
        return startPoint.squareDistance(left) < startPoint.squareDistance(right);
    }

    template<class T> HeightfieldPointHelper<T>::HeightfieldPointHelper(const std::vector<Point3<T>> &heightfieldPoints, unsigned int heightfieldXSize) :
            heightfieldPoints(heightfieldPoints),
            heightfieldXSize(heightfieldXSize)
    {
        heightfieldZSize = heightfieldPoints.size() / heightfieldXSize;

        xInterval = heightfieldPoints[1].X - heightfieldPoints[0].X;
        zInterval = heightfieldPoints[heightfieldXSize].Z - heightfieldPoints[0].Z;
    }

    /**
     * @return Nearest heightfield point (3D) of the provided coordinate point (2D)
     */
    template<class T> Point3<T> HeightfieldPointHelper<T>::findPointAt(const Point2<T> &point) const
    {
        Vector2<T> farLeftToPoint = Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z).vector(point);
        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftToPoint.X / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftToPoint.Y / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));

        return heightfieldPoints[xIndex + zIndex * heightfieldXSize];
    }

    /**
     * @return Heightfield height of the provided coordinate point (2D)
     */
    template<class T> T HeightfieldPointHelper<T>::findHeightAt(const Point2<T> &point) const
    {
        Vector2<T> farLeftToPoint = Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z).vector(point);
        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftToPoint.X / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftToPoint.Y / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));

        int nearestPointIndex = xIndex + zIndex * heightfieldXSize;
        Point3<T> nearestPoint = heightfieldPoints[nearestPointIndex];

        int leftRightPointXIndex = (nearestPoint.X > point.X) ? std::max(xIndex - 1, 0) : std::min(xIndex + 1, static_cast<int>(heightfieldXSize - 1));
        int leftRightPointIndex = leftRightPointXIndex + zIndex * heightfieldXSize;
        Point3<T> leftRightPoint = heightfieldPoints[leftRightPointIndex];

        int farNearPointZIndex = (nearestPoint.Z > point.Y) ? std::max(zIndex - 1, 0) : std::min(zIndex + 1, static_cast<int>(heightfieldZSize - 1));
        int farNearPointIndex = xIndex + farNearPointZIndex * heightfieldXSize;
        Point3<T> farNearPoint = heightfieldPoints[farNearPointIndex];

        if(nearestPointIndex==leftRightPointIndex || nearestPointIndex==farNearPointIndex || leftRightPointIndex==farNearPointIndex)
        { //coordinates are outside heightfield or on edge: return approximate point
            return nearestPoint.Y;
        }

        const Point3<T> coordinatePoint3D(point.X, 0.0, point.Y);
        Vector3<T> planeNormal = leftRightPoint.vector(farNearPoint).crossProduct(leftRightPoint.vector(nearestPoint)).normalize();
        return planeNormal.dotProduct(coordinatePoint3D.vector(nearestPoint)) / planeNormal.Y;
    }

    /**
     * @return Path points which follow the topography of the terrain between start and end point
     */
    template<class T> std::vector<Point3<T>> HeightfieldPointHelper<T>::followTopography(const Point3<T> &startPoint, const Point3<T> &endPoint) const
    {
        Point3<T> adjustedStartPoint = Point3<T>(startPoint.X, findHeightAt(Point2<T>(startPoint.X, startPoint.Z)), startPoint.Z);
        Point3<T> adjustedEndPoint = Point3<T>(endPoint.X, findHeightAt(Point2<T>(endPoint.X, endPoint.Z)), endPoint.Z);

        LineSegment2D<T> pathLine(Point2<T>(adjustedStartPoint.X, adjustedStartPoint.Z), Point2<T>(adjustedEndPoint.X, adjustedEndPoint.Z));
        DistanceToStartPointComp<T> distanceToStartPointComp(adjustedStartPoint);

        std::vector<Point3<T>> pathPoints;
        pathPoints.push_back(adjustedStartPoint);

        Vector2<T> farLeftToStartPoint = Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z).vector(Point2<T>(adjustedStartPoint.X, adjustedStartPoint.Z));
        Vector2<T> farLeftToEndPoint = Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z).vector(Point2<T>(adjustedEndPoint.X, adjustedEndPoint.Z));

        //X lines collision
        if(!isParallelToXAxis(pathLine, PARALLEL_EPSILON))
        {
            int xStartIndex = MathAlgorithm::clamp(static_cast<int>(std::floor(std::min(farLeftToStartPoint.X, farLeftToEndPoint.X)  / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));
            int xEndIndex = MathAlgorithm::clamp(static_cast<int>(std::ceil(std::max(farLeftToStartPoint.X, farLeftToEndPoint.X) / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));
            pathPoints.reserve(pathPoints.size() + (xEndIndex-xStartIndex) + 1);

            int zLastIndex = (heightfieldZSize - 1) * heightfieldXSize;
            for (int xCoord = xStartIndex; xCoord <= xEndIndex; ++xCoord)
            {
                Point3<T> firstLinePoint = heightfieldPoints[xCoord];
                Point3<T> endLinePoint = heightfieldPoints[xCoord + zLastIndex];
                LineSegment2D<T> xLine(Point2<T>(firstLinePoint.X, firstLinePoint.Z), Point2<T>(endLinePoint.X, endLinePoint.Z));

                addIntersectionPoint(xLine, pathLine, pathPoints);
            }
        }

        //Z lines collision
        if(!isParallelToZAxis(pathLine, PARALLEL_EPSILON))
        {
            int zStartIndex = MathAlgorithm::clamp(static_cast<int>(std::floor(std::min(farLeftToStartPoint.Y, farLeftToEndPoint.Y) / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));
            int zEndIndex = MathAlgorithm::clamp(static_cast<int>(std::ceil(std::max(farLeftToStartPoint.Y, farLeftToEndPoint.Y) / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));
            pathPoints.reserve(pathPoints.size() + (zEndIndex-zStartIndex) + 1);

            int xLastIndex = heightfieldXSize - 1;
            for(int zCoord = zStartIndex; zCoord <= zEndIndex; ++zCoord)
            {
                Point3<T> firstLinePoint = heightfieldPoints[zCoord * heightfieldXSize];
                Point3<T> endLinePoint = heightfieldPoints[xLastIndex + zCoord * heightfieldXSize];
                LineSegment2D<T> zLine(Point2<T>(firstLinePoint.X, firstLinePoint.Z), Point2<T>(endLinePoint.X, endLinePoint.Z));

                addIntersectionPoint(zLine, pathLine, pathPoints);
            }
        }

        //Oblique lines collision
        //Not implemented: no real added value and bad for performance

        std::sort(pathPoints.begin(), pathPoints.end(), distanceToStartPointComp);
        pathPoints.push_back(adjustedEndPoint);
        return pathPoints;
    }

    /**
     * @param pathPoints [out] Vector where intersection point is added
     */
    template<class T> void HeightfieldPointHelper<T>::addIntersectionPoint(const LineSegment2D<T> &line, const LineSegment2D<T> &pathLine, std::vector<Point3<T>> &pathPoints) const
    {
        bool hasIntersection;
        Point2<T> intersectionPoint = line.intersectPoint(pathLine, hasIntersection);
        if(hasIntersection)
        {
            T intersectionHeight = findHeightAt(intersectionPoint);
            pathPoints.push_back(Point3<T>(intersectionPoint.X, intersectionHeight, intersectionPoint.Y));
        }
    }

    template<class T> bool HeightfieldPointHelper<T>::isParallelToXAxis(const LineSegment2D<T> &pathLine, T epsilon) const
    {
        T dotProduct = pathLine.toVector().normalize().dotProduct(Vector2<T>(1.0, 0.0));
        return std::fabs(dotProduct) < epsilon;
    }

    template<class T> bool HeightfieldPointHelper<T>::isParallelToZAxis(const LineSegment2D<T> &pathLine, T epsilon) const
    {
        T dotProduct = pathLine.toVector().normalize().dotProduct(Vector2<T>(0.0, 1.0));
        return std::fabs(dotProduct) < epsilon;
    }

    //explicit template
    template class DistanceToStartPointComp<float>;
    template class HeightfieldPointHelper<float>;

    template class DistanceToStartPointComp<double>;
    template class HeightfieldPointHelper<double>;
}
