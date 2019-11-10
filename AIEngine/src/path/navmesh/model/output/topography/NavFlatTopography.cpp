#include "NavFlatTopography.h"

namespace urchin
{

    std::vector<Point3<float>> NavFlatTopography::followTopography(const Point3<float> &startPoint, const Point3<float> &endPoint) const
    {
        return {startPoint, endPoint};
    }

}
