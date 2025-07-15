#pragma once

#include <memory>
#include <vector>

#include <path/navmesh/model/output/NavPolygon.h>

namespace urchin {

    class NavObject {
        public:
            explicit NavObject();

            void addNavPolygons(const std::vector<std::shared_ptr<NavPolygon>>&);
            const std::vector<std::shared_ptr<NavPolygon>>& getNavPolygons() const;
            void removeAllNavPolygons();

        private:
            std::vector<std::shared_ptr<NavPolygon>> navPolygons;
    };

}
