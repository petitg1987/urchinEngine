#include <cmath>

#include "EdgeJumpResult.h"

namespace urchin
{

    EdgeJumpResult::EdgeJumpResult(bool bHasJumpRange, float jumpStartRange, float jumpEndRange) :
            bHasJumpRange(bHasJumpRange),
            jumpStartRange(jumpStartRange),
            jumpEndRange(jumpEndRange)
    {

    }

    EdgeJumpResult EdgeJumpResult::noEdgeJump()
    {
        return EdgeJumpResult(false, NAN, NAN);
    }

    EdgeJumpResult EdgeJumpResult::edgeJump(float jumpStartRange, float jumpEndRange)
    {
        return EdgeJumpResult(true, jumpStartRange, jumpEndRange);
    }

    bool EdgeJumpResult::hasJumpRange() const
    {
        return bHasJumpRange;
    }

    float EdgeJumpResult::getJumpStartRange() const
    {
        return jumpStartRange;
    }

    float EdgeJumpResult::getJumpEndRange() const
    {
        return jumpEndRange;
    }

}
