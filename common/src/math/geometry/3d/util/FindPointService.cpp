#include "FindPointService.h"

#include "math/algorithm/MathAlgorithm.h"
#include "math/geometry/3d/Plane.h"

namespace urchin
{

    /**
     * @return Nearest heightfield point (3D) of the provided coordinate point (2D)
     */
    template<class T> Point3<T> FindPointService<T>::findPointAt(const Point2<T> &coordinatePoint, const std::vector<Point3<T>> &heightfieldPoints, unsigned int heightfieldXSize) const
    { //TODO create instanciable class to save values 'heightfieldZSize', 'xInterval', 'zInterval'
        unsigned int heightfieldZSize = heightfieldPoints.size() / heightfieldXSize;
        Point2<T> farLeftCoordinate = coordinatePoint - Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z);

        T xInterval = heightfieldPoints[1].X - heightfieldPoints[0].X;
        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.X / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));

        T zInterval = heightfieldPoints[heightfieldXSize].Z - heightfieldPoints[0].Z;
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.Y / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));

        return heightfieldPoints[xIndex + zIndex * heightfieldXSize];
    }

    /**
     * @return Heightfield height of the provided coordinate point (2D)
     */
    template<class T> T FindPointService<T>::findHeightAt(const Point2<T> &coordinatePoint, const std::vector<Point3<T>> &heightfieldPoints, unsigned int heightfieldXSize) const
    {
        unsigned int heightfieldZSize = heightfieldPoints.size() / heightfieldXSize;
        Point2<T> farLeftCoordinate = coordinatePoint - Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z);

        T xInterval = heightfieldPoints[1].X - heightfieldPoints[0].X;
        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.X / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));

        T zInterval = heightfieldPoints[heightfieldXSize].Z - heightfieldPoints[0].Z;
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.Y / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));

        Point3<T> nearestPoint = heightfieldPoints[xIndex + zIndex * heightfieldXSize];

        int leftRightIndex = (nearestPoint.X > coordinatePoint.X) ? std::max(xIndex - 1, 0) : std::min(xIndex + 1, static_cast<int>(heightfieldXSize - 1));
        Point3<T> leftRightPoint = heightfieldPoints[leftRightIndex + zIndex * heightfieldXSize];

        int farNearPointIndex = (nearestPoint.Z > coordinatePoint.Y) ? std::max(zIndex - 1, 0) : std::min(zIndex + 1, static_cast<int>(heightfieldZSize - 1));
        Point3<T> farNearPoint = heightfieldPoints[xIndex + farNearPointIndex * heightfieldXSize];

        const Point3<T> coordinatePoint3D(coordinatePoint.X, 0.0, coordinatePoint.Y);
        Vector3<T> planeNormal = leftRightPoint.vector(farNearPoint).crossProduct(leftRightPoint.vector(nearestPoint)).normalize();
        return planeNormal.dotProduct(coordinatePoint3D.vector(nearestPoint)) / planeNormal.Y;
    }

    //explicit template
    template class FindPointService<float>;

    template class FindPointService<double>;
}
