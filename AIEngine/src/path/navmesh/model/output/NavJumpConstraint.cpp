#include <cassert>

#include "NavJumpConstraint.h"

namespace urchin
{
    NavJumpConstraint::NavJumpConstraint(float sourceEdgeStartPoint, float sourceEdgeEndPoint, float targetEdgeIndex) :
            sourceEdgeStartPoint(sourceEdgeStartPoint),
            sourceEdgeEndPoint(sourceEdgeEndPoint),
            targetEdgeIndex(targetEdgeIndex)
    {
        #ifdef _DEBUG
            assert(targetEdgeIndex <= 2);
            assert(sourceEdgeStartPoint >= sourceEdgeEndPoint);
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
}
