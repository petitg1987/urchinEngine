#include <path/navmesh/model/output/topography/NavTerrainTopography.h>

namespace urchin {
    NavTerrainTopography::NavTerrainTopography(std::unique_ptr<const HeightfieldPointHelper<float>> heightfieldPointHelper, const Point3<float>& terrainPosition) :
            heightfieldPointHelper(std::move(heightfieldPointHelper)),
            terrainPosition(terrainPosition) {

    }

    std::vector<Point3<float>> NavTerrainTopography::followTopography(const Point3<float>& startPoint, const Point3<float>& endPoint) const {
        std::vector<Point3<float>> topographyPoints = heightfieldPointHelper->followTopography(startPoint - terrainPosition, endPoint - terrainPosition);
        for (auto& topographyPoint : topographyPoints) {
            topographyPoint += terrainPosition;
        }
        return topographyPoints;
    }

}
