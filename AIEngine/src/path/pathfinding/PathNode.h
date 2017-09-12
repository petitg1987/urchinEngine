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
            PathNode(NavTriangleRef, unsigned int, unsigned int);

            const NavTriangleRef getTriangleRef() const;
            void setEdgeId(unsigned int);
            NavEdgeRef retrieveNavEdgeRef() const;

            void setGScore(float);
            float getGScore() const;
            float getHScore() const;
            float getFScore() const;

            void setPrevious(const std::shared_ptr<PathNode> &);
            const std::shared_ptr<PathNode> &getPrevious() const;

        private:
            NavTriangleRef triangleRef;
            int edgeId; //edge start ID of this PathNode leading to the next node

            float gScore;
            float hScore;

            std::shared_ptr<PathNode> previous;
    };

}

#endif
