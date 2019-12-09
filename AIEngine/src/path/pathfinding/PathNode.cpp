#include <cassert>
#include <utility>

#include "PathNode.h"

namespace urchin
{
    PathNode::PathNode(std::shared_ptr<NavTriangle> navTriangle, float gScore, float hScore) :
            navTriangle(std::move(navTriangle)),
            gScore(gScore),
            hScore(hScore)
    {

    }

    const std::shared_ptr<NavTriangle> &PathNode::getNavTriangle() const
    {
        return navTriangle;
    }

    void PathNode::setGScore(float gScore)
    {
        this->gScore = gScore;
    }

    float PathNode::getGScore() const
    {
        return gScore;
    }

    float PathNode::getHScore() const
    {
        return hScore;
    }

    float PathNode::getFScore() const
    {
        return gScore + hScore;
    }

    void PathNode::setPreviousNode(const std::shared_ptr<PathNode> &previousNode, const std::shared_ptr<NavLink> &navLink)
    {
        #ifdef _DEBUG
            assert(previousNode != nullptr);
            assert(navLink != nullptr);
        #endif

        this->previousNode = previousNode;
        this->navLink = navLink;
    }

    const std::shared_ptr<PathNode> &PathNode::getPreviousNode() const
    {
        return previousNode;
    }

    LineSegment3D<float> PathNode::computeEdgeWithPreviousNode() const
    {
        #ifdef _DEBUG
            assert(previousNode != nullptr);
            assert(navLink != nullptr);
        #endif

        return previousNode->getNavTriangle()->computeEdge(navLink->getSourceEdgeIndex());
    }

}
