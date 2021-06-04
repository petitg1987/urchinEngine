#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class PathPoint {
        public:
            PathPoint(const Point3<float>&, bool);

            const Point3<float>& getPoint() const;
            bool isJumpPoint() const;

        private:
            Point3<float> point;
            bool bIsJumpPoint;
    };

}
