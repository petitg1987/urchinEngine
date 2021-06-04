#pragma once

#include <vector>
#include <UrchinCommon.h>

namespace urchin {

    struct PlaneSurfaceSplit {
        std::vector<Point3<float>> planeSurfacePoints;
    };

    class PlaneSurfaceSplitService {
        public:
            explicit PlaneSurfaceSplitService(float);

            std::vector<PlaneSurfaceSplit> splitRectangleSurface(const std::vector<Point3<float>>&) const;

        private:
            float surfaceMaxSize;
    };

}
