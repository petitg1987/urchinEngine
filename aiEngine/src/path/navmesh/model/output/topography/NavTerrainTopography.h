#pragma once

#include <path/navmesh/model/output/topography/NavTopography.h>

namespace urchin {

    class NavTerrainTopography final : public NavTopography {
        public:
            NavTerrainTopography(std::unique_ptr<const HeightfieldPointHelper<float>>, const Point3<float>&);

            std::vector<Point3<float>> followTopography(const Point3<float>&, const Point3<float>&) const override;

        private:
            std::unique_ptr<const HeightfieldPointHelper<float>> heightfieldPointHelper;
            Point3<float> terrainPosition;
    };

}
