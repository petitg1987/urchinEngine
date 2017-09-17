#ifndef URCHINENGINE_FUNNELALGORITHM_H
#define URCHINENGINE_FUNNELALGORITHM_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

    class FunnelAlgorithm
    {
        public:
            std::vector<Point3<float>> determinePath(const std::vector<LineSegment3D<float>> &) const;
    };

}

#endif
