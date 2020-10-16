#include <cmath>
#include <cassert>

#include "EdgeLinkResult.h"

namespace urchin {

    EdgeLinkResult::EdgeLinkResult(bool bHasEdgesLink, bool bIsJumpLink, float linkStartRange, float linkEndRange) :
            bHasEdgesLink(bHasEdgesLink),
            bIsJumpLink(bIsJumpLink),
            linkStartRange(linkStartRange),
            linkEndRange(linkEndRange) {
        assert(!bIsJumpLink || bHasEdgesLink);
        assert(!bHasEdgesLink || linkStartRange > linkEndRange);
    }

    EdgeLinkResult EdgeLinkResult::noEdgeLink() {
        return EdgeLinkResult(false, false, NAN, NAN);
    }

    EdgeLinkResult EdgeLinkResult::collinearEdgeLink(float jumpStartRange, float jumpEndRange) {
        return EdgeLinkResult(true, false, jumpStartRange, jumpEndRange);
    }

    EdgeLinkResult EdgeLinkResult::edgeJump(float jumpStartRange, float jumpEndRange) {
        return EdgeLinkResult(true, true, jumpStartRange, jumpEndRange);
    }

    bool EdgeLinkResult::hasEdgesLink() const {
        return bHasEdgesLink;
    }

    bool EdgeLinkResult::isJumpLink() const {
        return bIsJumpLink;
    }

    float EdgeLinkResult::getLinkStartRange() const {
        return linkStartRange;
    }

    float EdgeLinkResult::getLinkEndRange() const {
        return linkEndRange;
    }

}
