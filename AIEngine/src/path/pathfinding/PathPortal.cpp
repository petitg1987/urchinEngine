#include "PathPortal.h"

#include <utility>

namespace urchin
{

    PathPortal::PathPortal(LineSegment3D<float> portal, std::shared_ptr<PathNode> previousPathNode, std::shared_ptr<PathNode> nextPathNode, bool bIsJumpPortal) :
        portal(std::move(portal)),
        previousPathNode(std::move(previousPathNode)),
        nextPathNode(std::move(nextPathNode)),
        bHasPivotPoint(false),
        bIsJumpPortal(bIsJumpPortal)
    {

    }

    void PathPortal::setPivotPoint(const Point3<float> &pivotPoint)
    {
        this->pivotPoint = pivotPoint;
        this->bHasPivotPoint = true;
    }

    bool PathPortal::hasPivotPoint() const
    {
        return bHasPivotPoint;
    }

    /**
     * @return pivot point. Pivot point represents a change of rotation in path or a change of polygon
     */
    const Point3<float> &PathPortal::getPivotPoint() const
    {
        return pivotPoint;
    }

    bool PathPortal::isJumpPortal() const
    {
        return bIsJumpPortal;
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
