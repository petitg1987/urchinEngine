#include <cassert>

#include <path/navmesh/link/EdgeLinkResult.h>

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
        return {false, false, 0.0f, 0.0f};
    }

    EdgeLinkResult EdgeLinkResult::collinearEdgeLink(float jumpStartRange, float jumpEndRange) {
        return {true, false, jumpStartRange, jumpEndRange};
    }

    EdgeLinkResult EdgeLinkResult::edgeJump(float jumpStartRange, float jumpEndRange) {
        return {true, true, jumpStartRange, jumpEndRange};
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
