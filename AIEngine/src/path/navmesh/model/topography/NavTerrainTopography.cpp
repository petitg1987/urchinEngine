#include "NavTerrainTopography.h"

namespace urchin
{
    NavTerrainTopography::NavTerrainTopography(std::shared_ptr<const HeightfieldPointHelper<float>> &heightfieldPointHelper) :
            heightfieldPointHelper(heightfieldPointHelper)
    {

    }


    std::vector<Point3<float>> NavTerrainTopography::followTopography(const Point3<float> &startPoint, const Point3<float> &endPoint) const
    {
        return heightfieldPointHelper->followTopography(startPoint, endPoint);
    }

}
