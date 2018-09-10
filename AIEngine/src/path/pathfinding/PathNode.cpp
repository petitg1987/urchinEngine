#include <cassert>

#include "PathNode.h"

namespace urchin
{
    PathNode::PathNode(const std::shared_ptr<NavTriangle> &navTriangle, float gScore, float hScore) :
            navTriangle(navTriangle),
            gScore(gScore),
            hScore(hScore),
            previousNodeLinkEdgeId(std::numeric_limits<unsigned int>::max())
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

    void PathNode::setPreviousNode(const std::shared_ptr<PathNode> &previousNode, unsigned int previousNodeLinkEdgeId)
    {
        #ifdef _DEBUG
            assert(previousNode != nullptr);
            assert(previousNodeLinkEdgeId <= 2);
        #endif

        this->previousNode = previousNode;
        this->previousNodeLinkEdgeId = previousNodeLinkEdgeId;
    }

    const std::shared_ptr<PathNode> &PathNode::getPreviousNode() const
    {
        return previousNode;
    }

    LineSegment3D<float> PathNode::computeEdgeWithPreviousNode() const
    {
        #ifdef _DEBUG
            assert(previousNode != nullptr);
            assert(previousNodeLinkEdgeId <= 2);
        #endif

        return previousNode->getNavTriangle()->computeEdge(previousNodeLinkEdgeId);
    }

}
