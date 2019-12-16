#include <cassert>

#include "NavJumpConstraint.h"

namespace urchin
{
    NavJumpConstraint::NavJumpConstraint(float sourceEdgeJumpStartRange, float sourceEdgeJumpEndRange, float targetEdgeIndex) :
            sourceEdgeJumpStartRange(sourceEdgeJumpStartRange),
            sourceEdgeJumpEndRange(sourceEdgeJumpEndRange),
            targetEdgeIndex(targetEdgeIndex)
    {
        #ifdef _DEBUG
            assert(targetEdgeIndex <= 2);
            assert(sourceEdgeJumpStartRange >= sourceEdgeJumpEndRange);
        #endif
    }

    float NavJumpConstraint::getSourceEdgeJumpStartRange() const
    {
        return sourceEdgeJumpStartRange;
    }

    float NavJumpConstraint::getSourceEdgeJumpEndRange() const
    {
        return sourceEdgeJumpEndRange;
    }

    float NavJumpConstraint::getTargetEdgeIndex() const
    {
        return targetEdgeIndex;
    }
}
