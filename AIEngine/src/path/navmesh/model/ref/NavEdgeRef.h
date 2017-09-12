#ifndef URCHINENGINE_NAVEDGEREF_H
#define URCHINENGINE_NAVEDGEREF_H

#include "path/navmesh/model/ref/NavTriangleRef.h"

namespace urchin
{

    class NavEdgeRef
    {
        public:
            NavEdgeRef(const NavTriangleRef &, unsigned int);

            const NavTriangleRef &getTriangleRef() const;
            unsigned int getEdgeIndex() const;

        private:
            NavTriangleRef triangleRef;
            unsigned int edgeIndex; //start point index of the edge
    };

}

#endif
