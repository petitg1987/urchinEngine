#include "NavLink.h"

#include "path/navmesh/model/output/NavPolygon.h"
#include "path/navmesh/model/output/NavTriangle.h"

namespace urchin
{
    NavLink::NavLink(NavLinkType linkType, unsigned int sourceEdgeIndex, const std::shared_ptr<NavTriangle> &targetTriangle, NavJumpConstraint *jumpConstraint) :
            linkType(linkType),
            sourceEdgeIndex(sourceEdgeIndex),
            targetTriangle(targetTriangle),
            jumpConstraint(jumpConstraint)
    {

    }

    NavLink::~NavLink()
    {
        delete jumpConstraint;
    }

    std::shared_ptr<NavLink> NavLink::newDirectLink(unsigned int sourceEdgeIndex, const std::shared_ptr<NavTriangle> &targetTriangle)
    {
        return std::shared_ptr<NavLink>(new NavLink(NavLinkType::DIRECT, sourceEdgeIndex, targetTriangle, nullptr));
    }

    std::shared_ptr<NavLink> NavLink::newJumpLink(unsigned int sourceEdgeIndex, const std::shared_ptr<NavTriangle> &targetTriangle, NavJumpConstraint *jumpConstraint)
    {
        assert(jumpConstraint != nullptr);

        return std::shared_ptr<NavLink>(new NavLink(NavLinkType::JUMP, sourceEdgeIndex, targetTriangle, jumpConstraint));
    }

    std::shared_ptr<NavLink> NavLink::copyLink(const std::shared_ptr<NavTriangle> &newTargetTriangle) const
    {
        auto *replicateJumpConstraint = jumpConstraint ? new NavJumpConstraint(*jumpConstraint) : nullptr;
        return std::shared_ptr<NavLink>(new NavLink(linkType, sourceEdgeIndex, newTargetTriangle, replicateJumpConstraint));
    }

    NavLinkType NavLink::getLinkType() const
    {
        return linkType;
    }

    unsigned int NavLink::getSourceEdgeIndex() const
    {
        return sourceEdgeIndex;
    }

    std::shared_ptr<NavTriangle> NavLink::getTargetTriangle() const
    {
        assert(!targetTriangle.expired());

        return targetTriangle.lock();
    }

    const NavJumpConstraint *NavLink::getJumpConstraint() const
    {
        assert(linkType == NavLinkType::JUMP);

        return jumpConstraint;
    }

}
