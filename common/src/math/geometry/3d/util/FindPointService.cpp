#include "FindPointService.h"

#include "math/algorithm/MathAlgorithm.h"

namespace urchin
{

    /**
     * @return Nearest heightfield point (3D) of the provided coordinate point (2D)
     */
    template<class T> Point3<T> FindPointService<T>::findPoint(const std::vector<Point3<T>> &heightfieldPoints, unsigned int heightfieldXSize, const Point2<T> &coordinatePoint) const
    {
        unsigned int heightfieldZSize = heightfieldPoints.size() / heightfieldXSize;
        Point2<T> farLeftCoordinate = coordinatePoint - Point2<T>(heightfieldPoints[0].X, heightfieldPoints[0].Z);

        T xInterval = heightfieldPoints[1].X - heightfieldPoints[0].X;
        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.X / xInterval)), 0, static_cast<int>(heightfieldXSize - 1));

        T zInterval = heightfieldPoints[heightfieldXSize].Z - heightfieldPoints[0].Z;
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.Y / zInterval)), 0, static_cast<int>(heightfieldZSize - 1));

        return heightfieldPoints[xIndex + zIndex * heightfieldXSize];
    }

    //explicit template
    template class FindPointService<float>;

    template class FindPointService<double>;
}
