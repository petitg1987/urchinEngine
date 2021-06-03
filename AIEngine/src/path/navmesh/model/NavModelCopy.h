#ifndef URCHINENGINE_NAVOUTPUTCOPY_H
#define URCHINENGINE_NAVOUTPUTCOPY_H

#include <memory>
#include <path/navmesh/model/output//NavPolygon.h>

namespace urchin {

    class NavModelCopy {
        public:
            static void copyNavPolygons(const std::vector<std::shared_ptr<NavPolygon>>&, std::vector<std::shared_ptr<NavPolygon>>&);
    };

}

#endif
