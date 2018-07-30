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
    template<class T> Point3<T> HeightfieldPointHelper<T>::findPointAt(const Point2<T> &coordinatePoint) const
    {
        Point2<T> farLeftCoordinate = coordinatePoint - Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z);
        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.X / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.Y / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));

        return heightfieldPoints[xIndex + zIndex * heightfieldXSize];
    }

    /**
     * @return Heightfield height of the provided coordinate point (2D)
     */
    template<class T> T HeightfieldPointHelper<T>::findHeightAt(const Point2<T> &coordinatePoint) const
    {
        Point2<T> farLeftCoordinate = coordinatePoint - Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z);
        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.X / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.Y / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));

        int nearestPointIndex = xIndex + zIndex * heightfieldXSize;
        Point3<T> nearestPoint = heightfieldPoints[nearestPointIndex];

        int leftRightPointXIndex = (nearestPoint.X > coordinatePoint.X) ? std::max(xIndex - 1, 0) : std::min(xIndex + 1, static_cast<int>(heightfieldXSize - 1));
        int leftRightPointIndex = leftRightPointXIndex + zIndex * heightfieldXSize;
        Point3<T> leftRightPoint = heightfieldPoints[leftRightPointIndex];

        int farNearPointZIndex = (nearestPoint.Z > coordinatePoint.Y) ? std::max(zIndex - 1, 0) : std::min(zIndex + 1, static_cast<int>(heightfieldZSize - 1));
        int farNearPointIndex = xIndex + farNearPointZIndex * heightfieldXSize;
        Point3<T> farNearPoint = heightfieldPoints[farNearPointIndex];

        if(nearestPointIndex==leftRightPointIndex || nearestPointIndex==farNearPointIndex || leftRightPointIndex==farNearPointIndex)
        { //coordinates are outside heightfield or on edge: return approximate point
            return nearestPoint.Y;
        }

        const Point3<T> coordinatePoint3D(coordinatePoint.X, 0.0, coordinatePoint.Y);
        Vector3<T> planeNormal = leftRightPoint.vector(farNearPoint).crossProduct(leftRightPoint.vector(nearestPoint)).normalize();
        return planeNormal.dotProduct(coordinatePoint3D.vector(nearestPoint)) / planeNormal.Y;
    }

    //explicit template
    template class HeightfieldPointHelper<float>;

    template class HeightfieldPointHelper<double>;
}
