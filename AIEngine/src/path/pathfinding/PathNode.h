#ifndef URCHINENGINE_PATHNODE_H
#define URCHINENGINE_PATHNODE_H

#include <memory>

#include "path/navmesh/model/NavTriangle.h"

namespace urchin
{

    class PathNode
    {
        public:
            PathNode(const std::shared_ptr<NavTriangle> &, float, float);

            const std::shared_ptr<NavTriangle> &getNavTriangle() const;

            void setGScore(float);
            float getGScore() const;
            float getHScore() const;
            float getFScore() const;

            void setPreviousNode(const std::shared_ptr<PathNode> &, unsigned int);
            const std::shared_ptr<PathNode> &getPreviousNode() const;
            LineSegment3D<float> computeEdgeWithPreviousNode() const;

        private:
            std::shared_ptr<NavTriangle> navTriangle;

            float gScore;
            float hScore;

            std::shared_ptr<PathNode> previousNode;
            unsigned int previousNodeLinkEdgeId; //edge ID on previous node which made link with this node
    };

}

#endif
