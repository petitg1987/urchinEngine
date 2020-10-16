#ifndef URCHINENGINE_NAVPOLYGONEDGE_H
#define URCHINENGINE_NAVPOLYGONEDGE_H

#include "path/navmesh/model/output/NavTriangle.h"

#include <memory>

namespace urchin {

    struct NavPolygonEdge {
        std::shared_ptr<NavTriangle> triangle;
        std::size_t edgeIndex;
    };

}

#endif
