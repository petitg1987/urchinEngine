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

    /**
     * @return Return crossing portals (edges) between previous PathNode and current PathNode
     */
    PathNodeEdgesLink PathNode::computePathNodeEdgesLink() const
    {
        #ifdef _DEBUG
            assert(previousNode != nullptr);
            assert(navLink != nullptr);
        #endif

        PathNodeEdgesLink pathNodeEdgesLink;

        if(navLink->getLinkType() == NavLinkType::DIRECT)
        {
            LineSegment3D<float> sourceAndTargetEdge = previousNode->getNavTriangle()->computeEdge(navLink->getSourceEdgeIndex());

            pathNodeEdgesLink.sourceEdge = sourceAndTargetEdge;
            pathNodeEdgesLink.targetEdge = sourceAndTargetEdge;
            pathNodeEdgesLink.areIdenticalEdges = true;
            return pathNodeEdgesLink;
        }else if(navLink->getLinkType() == NavLinkType::JUMP)
        {
            LineSegment3D<float> sourceEdge = previousNode->getNavTriangle()->computeEdge(navLink->getSourceEdgeIndex());
            LineSegment3D<float> targetEdge = getNavTriangle()->computeEdge(navLink->getJumpConstraint()->getTargetEdgeIndex());

            pathNodeEdgesLink.sourceEdge = LineSegment3D<float>(
                    navLink->getJumpConstraint()->getSourceEdgeStartPoint() * sourceEdge.getA() + (1.0f - navLink->getJumpConstraint()->getSourceEdgeStartPoint()) * sourceEdge.getB(),
                    navLink->getJumpConstraint()->getSourceEdgeEndPoint() * sourceEdge.getA() + (1.0f - navLink->getJumpConstraint()->getSourceEdgeEndPoint()) * sourceEdge.getB());
            pathNodeEdgesLink.targetEdge = LineSegment3D<float>(
                    targetEdge.closestPoint(pathNodeEdgesLink.sourceEdge.getA()),
                    targetEdge.closestPoint(pathNodeEdgesLink.sourceEdge.getB()));
            pathNodeEdgesLink.areIdenticalEdges = false;
            return pathNodeEdgesLink;
        }

        throw std::runtime_error("Unknown link type: " + std::to_string(navLink->getLinkType()));
    }

}
