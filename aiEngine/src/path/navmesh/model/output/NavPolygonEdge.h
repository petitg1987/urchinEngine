#pragma once

#include "path/navmesh/model/output/NavTriangle.h"

#include <memory>

namespace urchin {

    struct NavPolygonEdge {
        std::shared_ptr<NavTriangle> triangle;
        std::size_t edgeIndex;
    };

}
