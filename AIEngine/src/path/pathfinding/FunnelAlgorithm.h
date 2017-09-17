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
            const std::vector<Point3<float>> &determinePath(const std::vector<LineSegment3D<float>> &);

        private:
            enum FunnelSide
            {
                LEFT,
                RIGHT
            };

            void updateFunnelSide(FunnelSide, unsigned int, const std::vector<LineSegment3D<float>> &);
            void updateSideIndex(FunnelSide, unsigned int);

            Point3<float> apex;
            std::pair<unsigned int, unsigned int> sideIndices; //first: left index, second: right index
            std::vector<Point3<float>> path;
    };

}

#endif
