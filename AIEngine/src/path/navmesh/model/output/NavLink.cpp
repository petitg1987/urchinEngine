#include "NavLink.h"

#include "path/navmesh/model/output/NavPolygon.h"
#include "path/navmesh/model/output/NavTriangle.h"

namespace urchin {
    NavLink::NavLink(NavLinkType linkType, unsigned int sourceEdgeIndex, const std::shared_ptr<NavTriangle>& targetTriangle, NavLinkConstraint* linkConstraint) :
            linkType(linkType),
            sourceEdgeIndex(sourceEdgeIndex),
            targetTriangle(targetTriangle),
            linkConstraint(linkConstraint) {

    }

    NavLink::~NavLink() {
        delete linkConstraint;
    }

    std::shared_ptr<NavLink> NavLink::newStandardLink(unsigned int sourceEdgeIndex, const std::shared_ptr<NavTriangle>& targetTriangle) {
        return std::shared_ptr<NavLink>(new NavLink(NavLinkType::STANDARD, sourceEdgeIndex, targetTriangle, nullptr));
    }

    std::shared_ptr<NavLink> NavLink::newJoinPolygonsLink(unsigned int sourceEdgeIndex, const std::shared_ptr<NavTriangle>& targetTriangle, NavLinkConstraint* linkConstraint) {
        assert(linkConstraint != nullptr);
        return std::shared_ptr<NavLink>(new NavLink(NavLinkType::JOIN_POLYGONS, sourceEdgeIndex, targetTriangle, linkConstraint));
    }

    std::shared_ptr<NavLink> NavLink::newJumpLink(unsigned int sourceEdgeIndex, const std::shared_ptr<NavTriangle>& targetTriangle, NavLinkConstraint* linkConstraint) {
        assert(linkConstraint != nullptr);
        return std::shared_ptr<NavLink>(new NavLink(NavLinkType::JUMP, sourceEdgeIndex, targetTriangle, linkConstraint));
    }

    std::shared_ptr<NavLink> NavLink::copyLink(const std::shared_ptr<NavTriangle>& newTargetTriangle) const {
        auto *replicateLinkConstraint = linkConstraint ? new NavLinkConstraint(*linkConstraint) : nullptr;
        return std::shared_ptr<NavLink>(new NavLink(linkType, sourceEdgeIndex, newTargetTriangle, replicateLinkConstraint));
    }

    NavLinkType NavLink::getLinkType() const {
        return linkType;
    }

    unsigned int NavLink::getSourceEdgeIndex() const {
        return sourceEdgeIndex;
    }

    std::shared_ptr<NavTriangle> NavLink::getTargetTriangle() const {
        assert(!targetTriangle.expired());

        return targetTriangle.lock();
    }

    const NavLinkConstraint *NavLink::getLinkConstraint() const {
        assert(linkType == NavLinkType::JUMP || linkType == NavLinkType::JOIN_POLYGONS);

        return linkConstraint;
    }

}
