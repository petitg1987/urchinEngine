#ifndef URCHINENGINE_EDGEJUMPDETECTION_H
#define URCHINENGINE_EDGEJUMPDETECTION_H

#include "UrchinCommon.h"

#include "path/navmesh/jump/EdgeJumpResult.h"

namespace urchin
{

    class EdgeJumpDetection
    {
        public:
            explicit EdgeJumpDetection(float);

            EdgeJumpResult detectJump(const LineSegment3D<float> &, const LineSegment3D<float> &) const;

        private:
            float jumpMaxLength;
            float jumpMaxSquareLength;

            bool canJumpThatFar(const Point3<float> &, const Point3<float> &) const;
            bool isProperJumpDirection(const LineSegment3D<float> &, const LineSegment3D<float> &, const Point3<float> &, const Point3<float> &) const;
    };

}

#endif
