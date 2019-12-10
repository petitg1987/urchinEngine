#include "NavLink.h"
#include "path/navmesh/model/output/NavPolygon.h"
#include "path/navmesh/model/output/NavTriangle.h"

namespace urchin
{
    NavLink::NavLink(NavLinkType linkType, unsigned int sourceEdgeIndex, const std::shared_ptr<NavPolygon> &targetPolygon,
            const std::shared_ptr<NavTriangle> &targetTriangle, JumpConstraint *jumpConstraint) :
            linkType(linkType),
            sourceEdgeIndex(sourceEdgeIndex),
            targetPolygon(targetPolygon),
            targetTriangle(targetTriangle),
            jumpConstraint(jumpConstraint)
    {

    }

    NavLink::~NavLink()
    {
        delete jumpConstraint;
    }

    std::shared_ptr<NavLink> NavLink::newDirectLink(unsigned int sourceEdgeIndex, const std::shared_ptr<NavPolygon> &targetPolygon,
            const std::shared_ptr<NavTriangle> &targetTriangle)
    {
        return std::shared_ptr<NavLink>(new NavLink(NavLinkType::DIRECT, sourceEdgeIndex, targetPolygon, targetTriangle, nullptr));
    }

    std::shared_ptr<NavLink> NavLink::newJumpLink(unsigned int sourceEdgeIndex, const std::shared_ptr<NavPolygon> &targetPolygon,
            const std::shared_ptr<NavTriangle> &targetTriangle, JumpConstraint *jumpConstraint)
    {
        #ifdef _DEBUG
            assert(jumpConstraint != nullptr);
            assert(jumpConstraint->sourceEdgeStartPoint <= jumpConstraint->sourceEdgeEndPoint);
            assert(jumpConstraint->targetEdgeStartPoint <= jumpConstraint->targetEdgeEndPoint);
        #endif

        return std::shared_ptr<NavLink>(new NavLink(NavLinkType::JUMP, sourceEdgeIndex, targetPolygon, targetTriangle, jumpConstraint));
    }

    NavLinkType NavLink::getLinkType() const
    {
        return linkType;
    }

    const JumpConstraint *NavLink::getJumpConstraint() const
    {
        #ifdef _DEBUG
            assert(linkType == NavLinkType::JUMP);
        #endif

        return jumpConstraint;
    }

    unsigned int NavLink::getSourceEdgeIndex() const
    {
        return sourceEdgeIndex;
    }

    std::shared_ptr<NavPolygon> NavLink::getTargetPolygon() const
    {
        return targetPolygon.lock();
    }

    std::shared_ptr<NavTriangle> NavLink::getTargetTriangle() const
    {
        return targetTriangle.lock();
    }

}
