#include <cassert>
#include <utility>

#include "PathNode.h"

namespace urchin {
    PathNode::PathNode(std::shared_ptr<NavTriangle> navTriangle, float gScore, float hScore) :
            navTriangle(std::move(navTriangle)),
            gScore(gScore),
            hScore(hScore) {

    }

    const std::shared_ptr<NavTriangle> &PathNode::getNavTriangle() const {
        return navTriangle;
    }

    void PathNode::setGScore(float gScore) {
        this->gScore = gScore;
    }

    float PathNode::getGScore() const {
        return gScore;
    }

    float PathNode::getHScore() const {
        return hScore;
    }

    float PathNode::getFScore() const {
        return gScore + hScore;
    }

    void PathNode::setPreviousNode(const std::shared_ptr<PathNode> &previousNode, const std::shared_ptr<NavLink> &navLink) {
        assert(previousNode != nullptr);
        assert(navLink != nullptr);

        this->previousNode = previousNode;
        this->navLink = navLink;
    }

    const std::shared_ptr<PathNode> &PathNode::getPreviousNode() const {
        return previousNode;
    }

    /**
     * @return Return crossing portals (edges) between previous PathNode and current PathNode
     */
    PathNodeEdgesLink PathNode::computePathNodeEdgesLink() const {
        assert(previousNode != nullptr);
        assert(navLink != nullptr);

        PathNodeEdgesLink pathNodeEdgesLink;

        if(navLink->getLinkType() == NavLinkType::STANDARD) {
            LineSegment3D<float> sourceAndTargetEdge = previousNode->getNavTriangle()->computeEdge(navLink->getSourceEdgeIndex());

            pathNodeEdgesLink.sourceEdge = sourceAndTargetEdge;
            pathNodeEdgesLink.targetEdge = sourceAndTargetEdge;
            pathNodeEdgesLink.areIdenticalEdges = true;

            return pathNodeEdgesLink;
        } else if(navLink->getLinkType() == NavLinkType::JOIN_POLYGONS) {
            LineSegment3D<float> sourceEdge = previousNode->getNavTriangle()->computeEdge(navLink->getSourceEdgeIndex());
            LineSegment3D<float> polygonJoinEdge = navLink->getLinkConstraint()->computeSourceJumpEdge(sourceEdge);

            pathNodeEdgesLink.sourceEdge = polygonJoinEdge;
            pathNodeEdgesLink.targetEdge = polygonJoinEdge;
            pathNodeEdgesLink.areIdenticalEdges = true;

            return pathNodeEdgesLink;
        } else if(navLink->getLinkType() == NavLinkType::JUMP) {
            LineSegment3D<float> sourceEdge = previousNode->getNavTriangle()->computeEdge(navLink->getSourceEdgeIndex());

            pathNodeEdgesLink.sourceEdge = navLink->getLinkConstraint()->computeSourceJumpEdge(sourceEdge);
            pathNodeEdgesLink.targetEdge = getNavTriangle()->computeEdge(navLink->getLinkConstraint()->getTargetEdgeIndex());
            pathNodeEdgesLink.areIdenticalEdges = false;

            return pathNodeEdgesLink;
        }

        throw std::runtime_error("Unknown link type: " + std::to_string(navLink->getLinkType()));
    }

}
