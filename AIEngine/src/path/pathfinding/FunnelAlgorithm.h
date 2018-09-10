#ifndef URCHINENGINE_FUNNELALGORITHM_H
#define URCHINENGINE_FUNNELALGORITHM_H

#include <vector>
#include <utility>
#include "UrchinCommon.h"

#include "path/pathfinding/PathPortal.h"

namespace urchin
{

    class FunnelAlgorithm
    {
        public:
            explicit FunnelAlgorithm(const std::vector<std::shared_ptr<PathPortal>> &);

            const std::vector<Point3<float>> &findPath();

        private:
            enum FunnelSide
            {
                LEFT,
                RIGHT
            };

            int updateFunnelSide(FunnelSide, unsigned int);
            const Point3<float> &getPortalPoint(FunnelSide, unsigned int) const;
            void updateSideIndex(FunnelSide, unsigned int);

            std::vector<std::shared_ptr<PathPortal>> portals;

            Point3<float> apex;
            std::pair<unsigned int, unsigned int> sideIndices; //first: left index, second: right index
            std::vector<Point3<float>> path;
    };

}

#endif
