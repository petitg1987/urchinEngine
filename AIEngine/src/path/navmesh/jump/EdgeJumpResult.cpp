#include <cmath>
#include <cassert>

#include "EdgeJumpResult.h"

namespace urchin
{

    EdgeJumpResult::EdgeJumpResult(bool bIsCollinearEdgeJump, bool bHasJumpRange, float jumpStartRange, float jumpEndRange) :
            bIsCollinearEdgeJump(bIsCollinearEdgeJump),
            bHasJumpRange(bHasJumpRange),
            jumpStartRange(jumpStartRange),
            jumpEndRange(jumpEndRange)
    {
        assert(!bHasJumpRange || jumpStartRange >= jumpEndRange);
    }

    EdgeJumpResult EdgeJumpResult::noEdgeJump()
    {
        return EdgeJumpResult(false, false, NAN, NAN);
    }

    EdgeJumpResult EdgeJumpResult::collinearNoEdgeJump()
    {
        return EdgeJumpResult(true, false, NAN, NAN);
    }

    EdgeJumpResult EdgeJumpResult::edgeJump(float jumpStartRange, float jumpEndRange)
    {
        return EdgeJumpResult(false, true, jumpStartRange, jumpEndRange);
    }

    EdgeJumpResult EdgeJumpResult::collinearEdgeJump(float jumpStartRange, float jumpEndRange)
    {
        return EdgeJumpResult(true, true, jumpStartRange, jumpEndRange);
    }

    bool EdgeJumpResult::isCollinearEdgeJump() const
    { //TODO use this method
        return bIsCollinearEdgeJump;
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
