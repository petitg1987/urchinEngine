#include <utility>

#include "PathPortal.h"
#include "path/navmesh/model/output/NavPolygon.h"

namespace urchin
{

    PathPortal::PathPortal(LineSegment3D<float> portal, std::shared_ptr<PathNode> previousPathNode, std::shared_ptr<PathNode> nextPathNode, bool bIsJumpOriginPortal) :
        portal(std::move(portal)),
        previousPathNode(std::move(previousPathNode)),
        nextPathNode(std::move(nextPathNode)),
        bHasTransitionPoint(false),
        bIsJumpOriginPortal(bIsJumpOriginPortal)
    {

    }

    void PathPortal::setTransitionPoint(const Point3<float> &transitionPoint)
    {
        this->transitionPoint = transitionPoint;
        this->bHasTransitionPoint = true;
    }

    bool PathPortal::hasTransitionPoint() const
    {
        return bHasTransitionPoint;
    }

    /**
     * Transition point represents a change of state: pivot/rotation, change of polygon, jump...
     */
    const Point3<float> &PathPortal::getTransitionPoint() const
    {
        #ifdef _DEBUG
            assert(bHasTransitionPoint);
        #endif

        return transitionPoint;
    }

    bool PathPortal::isJumpOriginPortal() const
    {
        return bIsJumpOriginPortal;
    }

    bool PathPortal::isBetweenTwoPolygons() const
    {
        return previousPathNode->getNavTriangle()->getNavPolygon()->getName() != nextPathNode->getNavTriangle()->getNavPolygon()->getName();
    }

    const LineSegment3D<float> &PathPortal::getPortal() const
    {
        return portal;
    }

    const std::shared_ptr<PathNode> &PathPortal::getPreviousPathNode() const
    {
        return previousPathNode;
    }

    const std::shared_ptr<PathNode> &PathPortal::getNextPathNode() const
    {
        return nextPathNode;
    }

}
