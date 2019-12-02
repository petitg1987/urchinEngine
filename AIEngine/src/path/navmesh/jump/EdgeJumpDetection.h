#ifndef URCHINENGINE_EDGEJUMPDETECTION_H
#define URCHINENGINE_EDGEJUMPDETECTION_H

#include "UrchinCommon.h"

namespace urchin
{

    class EdgeJumpDetection
    {
        public:
            void detectJump(const LineSegment3D<float> &, const LineSegment3D<float> &, float) const;

    };

}

#endif
