#include "NavLink.h"

#include <utility>
#include "path/navmesh/model/output/NavPolygon.h"
#include "path/navmesh/model/output/NavTriangle.h"

namespace urchin
{
    NavLink::NavLink(NavLinkType linkType, unsigned int sourceEdgeIndex, std::shared_ptr<NavTriangle> targetTriangle, JumpConstraint *jumpConstraint) :
            linkType(linkType),
            sourceEdgeIndex(sourceEdgeIndex),
            targetTriangle(std::move(targetTriangle)),
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

    std::shared_ptr<NavLink> NavLink::newJumpLink(unsigned int sourceEdgeIndex, const std::shared_ptr<NavTriangle> &targetTriangle, JumpConstraint *jumpConstraint)
    {
        #ifdef _DEBUG
            assert(jumpConstraint != nullptr);
            assert(jumpConstraint->sourceEdgeStartPoint <= jumpConstraint->sourceEdgeEndPoint);
            assert(jumpConstraint->targetEdgeStartPoint <= jumpConstraint->targetEdgeEndPoint);
        #endif

        return std::shared_ptr<NavLink>(new NavLink(NavLinkType::JUMP, sourceEdgeIndex, targetTriangle, jumpConstraint));
    }

    NavLinkType NavLink::getLinkType() const
    {
        return linkType;
    }

    unsigned int NavLink::getSourceEdgeIndex() const
    {
        return sourceEdgeIndex;
    }

    const std::shared_ptr<NavTriangle> &NavLink::getTargetTriangle() const
    {
        return targetTriangle;
    }

    const JumpConstraint *NavLink::getJumpConstraint() const
    {
        #ifdef _DEBUG
            assert(linkType == NavLinkType::JUMP);
        #endif

        return jumpConstraint;
    }

}
