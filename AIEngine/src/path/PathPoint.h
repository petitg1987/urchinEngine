#ifndef URCHINENGINE_PATHPOINT_H
#define URCHINENGINE_PATHPOINT_H

#include "UrchinCommon.h"

namespace urchin
{

    class PathPoint
    {
        public:
            PathPoint(const Point3<float> &, bool);

            const Point3<float> &getPoint() const;
            bool isTurnPoint() const;

        private:
            Point3<float> point;
            bool bIsTurnPoint;

    };

}

#endif
