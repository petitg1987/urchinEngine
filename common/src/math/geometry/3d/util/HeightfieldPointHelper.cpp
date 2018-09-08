#include "HeightfieldPointHelper.h"

#include "math/algorithm/MathAlgorithm.h"
#include "math/geometry/3d/Plane.h"

namespace urchin
{

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
    template<class T> std::vector<Point3<T>> HeightfieldPointHelper<T>::establishPath(const Point3<T> &startPoint, const Point3<T> &endPoint) const
    {
        std::vector<Point3<T>> pathPoints;
        pathPoints.push_back(startPoint); //TODO determine Y

        Vector2<T> farLeftToStartPoint = Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z).vector(Point2<T>(startPoint.X, startPoint.Z));
        Vector2<T> farLeftToEndPoint = Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z).vector(Point2<T>(endPoint.X, endPoint.Z));

        //X lines collision
        int xStartIndex = MathAlgorithm::clamp(static_cast<int>(std::floor(farLeftToStartPoint.X / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));
        int xEndIndex = MathAlgorithm::clamp(static_cast<int>(std::ceil(farLeftToEndPoint.X / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));
        //TODO implements

        //Z lines collision
        int zStartIndex = MathAlgorithm::clamp(static_cast<int>(std::floor(farLeftToStartPoint.Y / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));
        int zEndIndex = MathAlgorithm::clamp(static_cast<int>(std::ceil(farLeftToEndPoint.Y / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));
        //TODO implements

        //Oblique lines collision
        //TODO implements

        pathPoints.push_back(endPoint); //TODO determine Y
        return pathPoints;
    }

    //explicit template
    template class HeightfieldPointHelper<float>;

    template class HeightfieldPointHelper<double>;
}
