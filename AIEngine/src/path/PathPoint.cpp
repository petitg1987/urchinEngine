#include "PathPoint.h"

namespace urchin
{

    PathPoint::PathPoint(const Point3<float> &point, bool bIsTurnPoint) :
        point(point),
        bIsTurnPoint(bIsTurnPoint)
    {

    }

    const Point3<float> &PathPoint::getPoint() const
    {
        return point;
    }

    /**
     * @return True when this point represent a turn in path
     */
    bool PathPoint::isTurnPoint() const
    {
        return bIsTurnPoint;
    }

}
