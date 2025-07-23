#pragma once

#include <vector>
#include <utility>
#include <UrchinCommon.h>

#include "path/pathfinding/PathPortal.h"

namespace urchin {

    class FunnelAlgorithm {
        public:
            void computePivotPoints(const std::vector<std::unique_ptr<PathPortal>>&);

        private:
            enum FunnelSide {
                LEFT,
                RIGHT
            };

            int updateFunnelSide(const std::vector<std::unique_ptr<PathPortal>>&, FunnelSide, unsigned int);
            const Point3<float>& getPortalPoint(const std::vector<std::unique_ptr<PathPortal>>&, FunnelSide, unsigned int) const;
            void updateSideIndex(FunnelSide, unsigned int);

            Point3<float> apex;
            std::pair<unsigned int, unsigned int> sideIndices; //first: left index, second: right index
    };

}
