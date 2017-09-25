#include <cassert>

#include "PathNode.h"

namespace urchin
{
    PathNode::PathNode(NavTriangleRef triangleRef, unsigned int gScore, unsigned int hScore) :
            triangleRef(triangleRef),
            gScore(gScore),
            hScore(hScore),
            previousNodeLinkEdgeId(-1)
    {

    }

    const NavTriangleRef PathNode::getTriangleRef() const
    {
        return triangleRef;
    }

    NavEdgeRef PathNode::retrieveNavEdgeRef() const
    {
        #ifdef _DEBUG
            assert(previousNodeLinkEdgeId != -1);
            assert(previousNode!=nullptr);
        #endif

        return {previousNode->getTriangleRef(), static_cast<unsigned int>(previousNodeLinkEdgeId)};
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
        this->previousNode = previousNode;
        this->previousNodeLinkEdgeId = previousNodeLinkEdgeId;
    }

    const std::shared_ptr<PathNode> &PathNode::getPreviousNode() const
    {
        return previousNode;
    }

    int PathNode::getPreviousNodeLinkEdgeId() const
    {
        return previousNodeLinkEdgeId;
    }

}
