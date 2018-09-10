#include "PathPortal.h"

namespace urchin
{

    PathPortal::PathPortal(const LineSegment3D<float> &portal, const std::shared_ptr<PathNode> &prevPathNode, const std::shared_ptr<PathNode> &nextPathNode) :
        portal(portal),
        prevPathNode(prevPathNode),
        nextPathNode(nextPathNode)
    {

    }

    const LineSegment3D<float> &PathPortal::getPortal() const
    {
        return portal;
    }

    const std::shared_ptr<PathNode> &PathPortal::getPreviousPathNode() const
    {
        return prevPathNode;
    }

    const std::shared_ptr<PathNode> &PathPortal::getNextPathNode() const
    {
        return nextPathNode;
    }

}
