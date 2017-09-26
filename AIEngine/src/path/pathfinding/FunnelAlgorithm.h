#ifndef URCHINENGINE_FUNNELALGORITHM_H
#define URCHINENGINE_FUNNELALGORITHM_H

#include <vector>
#include <utility>
#include "UrchinCommon.h"

namespace urchin
{

    class FunnelAlgorithm
    {
        public:
            FunnelAlgorithm(const std::shared_ptr<std::vector<LineSegment3D<float>>> &);

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

            std::shared_ptr<std::vector<LineSegment3D<float>>> portals;

            Point3<float> apex;
            std::pair<unsigned int, unsigned int> sideIndices; //first: left index, second: right index
            std::vector<Point3<float>> path;
    };

}

#endif
