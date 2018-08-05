#ifndef URCHINENGINE_PATHNODE_H
#define URCHINENGINE_PATHNODE_H

#include <memory>

#include "path/navmesh/model/ref/NavTriangleRef.h"
#include "path/navmesh/model/ref/NavEdgeRef.h"

namespace urchin
{

    class PathNode
    {
        public:
            PathNode(NavTriangleRef, float, float);

            const NavTriangleRef getTriangleRef() const;
            NavEdgeRef retrieveNavEdgeRef() const;

            void setGScore(float);
            float getGScore() const;
            float getHScore() const;
            float getFScore() const;

            void setPreviousNode(const std::shared_ptr<PathNode> &, unsigned int);
            const std::shared_ptr<PathNode> &getPreviousNode() const;
            int getPreviousNodeLinkEdgeId() const;

        private:
            NavTriangleRef triangleRef;

            float gScore;
            float hScore;

            std::shared_ptr<PathNode> previousNode;
            int previousNodeLinkEdgeId; //edge ID on previous node which made link with this node
    };

}

#endif
