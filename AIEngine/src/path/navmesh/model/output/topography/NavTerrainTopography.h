#pragma once

#include <path/navmesh/model/output/topography/NavTopography.h>

namespace urchin {

    class NavTerrainTopography : public NavTopography {
        public:
            NavTerrainTopography(std::shared_ptr<const HeightfieldPointHelper<float>>&, const Point3<float>&);

            std::vector<Point3<float>> followTopography(const Point3<float>&, const Point3<float>&) const override;

        private:
            std::shared_ptr<const HeightfieldPointHelper<float>> heightfieldPointHelper;
            Point3<float> terrainPosition;
    };

}
