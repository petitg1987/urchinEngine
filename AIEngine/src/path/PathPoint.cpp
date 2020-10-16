#include "PathPoint.h"

namespace urchin {

    PathPoint::PathPoint(const Point3<float> &point, bool bIsJumpPoint) :
        point(point),
        bIsJumpPoint(bIsJumpPoint) {

    }

    const Point3<float> &PathPoint::getPoint() const {
        return point;
    }

    /**
     * @return True when this point represent a jump to next point
     */
    bool PathPoint::isJumpPoint() const {
        return bIsJumpPoint;
    }

}
