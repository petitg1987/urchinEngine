#ifndef URCHINENGINE_PATHNODE_H
#define URCHINENGINE_PATHNODE_H

#include <memory>

#include "path/navmesh/model/output/NavTriangle.h"

namespace urchin {

    /**
     * Represent a link between two PathNode.
     */
    struct PathNodeEdgesLink {
        LineSegment3D<float> sourceEdge;
        LineSegment3D<float> targetEdge;
        bool areIdenticalEdges = true;
    };

    class PathNode {
        public:
            PathNode(std::shared_ptr<NavTriangle>, float, float);

            const std::shared_ptr<NavTriangle>& getNavTriangle() const;

            void setGScore(float);
            float getGScore() const;
            float getHScore() const;
            float getFScore() const;

            void setPreviousNode(const std::shared_ptr<PathNode>&, const std::shared_ptr<NavLink>&);
            const std::shared_ptr<PathNode>& getPreviousNode() const;
            PathNodeEdgesLink computePathNodeEdgesLink() const;

        private:
            std::shared_ptr<NavTriangle> navTriangle;

            float gScore;
            float hScore;

            std::shared_ptr<PathNode> previousNode;
            std::shared_ptr<NavLink> navLink; //link between previousNode and this
    };

}

#endif
