#include <cassert>

#include "PathNode.h"

namespace urchin
{
    PathNode::PathNode(NavTriangleRef triangleRef, unsigned int gScore, unsigned int hScore) :
            triangleRef(triangleRef),
            gScore(gScore),
            hScore(hScore),
            edgeId(-1)
    {

    }

    const NavTriangleRef PathNode::getTriangleRef() const
    {
        return triangleRef;
    }

    void PathNode::setEdgeId(unsigned int edgeId)
    {
        this->edgeId = edgeId;
    }

    NavEdgeRef PathNode::retrieveNavEdgeRef() const
    {
        #ifdef _DEBUG
            assert(edgeId != -1);
        #endif

        return {triangleRef, static_cast<unsigned int>(edgeId)};
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

    void PathNode::setPrevious(const std::shared_ptr<PathNode> &previous)
    {
        this->previous = previous;
    }

    const std::shared_ptr<PathNode> &PathNode::getPrevious() const
    {
        return previous;
    }

}
