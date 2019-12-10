#include <cassert>

#include "NavJumpConstraint.h"

namespace urchin
{
    NavJumpConstraint::NavJumpConstraint(float sourceEdgeStartPoint, float sourceEdgeEndPoint, float targetEdgeIndex, float targetEdgeStartPoint, float targetEdgeEndPoint) :
            sourceEdgeStartPoint(sourceEdgeStartPoint),
            sourceEdgeEndPoint(sourceEdgeEndPoint),
            targetEdgeIndex(targetEdgeIndex),
            targetEdgeStartPoint(targetEdgeStartPoint),
            targetEdgeEndPoint(targetEdgeEndPoint)
    {
        #ifdef _DEBUG
            assert(targetEdgeIndex <= 2);
            assert(sourceEdgeStartPoint >= sourceEdgeEndPoint);
            assert(targetEdgeStartPoint >= targetEdgeEndPoint);
        #endif
    }

    float NavJumpConstraint::getSourceEdgeStartPoint() const
    {
        return sourceEdgeStartPoint;
    }

    float NavJumpConstraint::getSourceEdgeEndPoint() const
    {
        return sourceEdgeEndPoint;
    }

    float NavJumpConstraint::getTargetEdgeIndex() const
    {
        return targetEdgeIndex;
    }

    float NavJumpConstraint::getTargetEdgeStartPoint() const
    {
        return targetEdgeStartPoint;
    }

    float NavJumpConstraint::getTargetEdgeEndPoint() const
    {
        return targetEdgeEndPoint;
    }
}
