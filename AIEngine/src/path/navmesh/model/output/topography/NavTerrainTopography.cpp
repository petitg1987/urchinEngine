#include "NavTerrainTopography.h"

namespace urchin {
    NavTerrainTopography::NavTerrainTopography(std::shared_ptr<const HeightfieldPointHelper<float>>& heightfieldPointHelper, const Point3<float>& terrainPosition) :
            heightfieldPointHelper(heightfieldPointHelper),
            terrainPosition(terrainPosition) {

    }


    std::vector<Point3<float>> NavTerrainTopography::followTopography(const Point3<float>& startPoint, const Point3<float>& endPoint) const {
        std::vector<Point3<float>> topographyPoints = heightfieldPointHelper->followTopography(startPoint - terrainPosition, endPoint - terrainPosition);
        for (auto &topographyPoint : topographyPoints) {
            topographyPoint += terrainPosition;
        }
        return topographyPoints;
    }

}
