#pragma once

#include <vector>
#include <memory>

#include "path/navmesh/model/output/NavPolygon.h"

namespace urchin {

    /**
     * Navigation mesh of world which can be used to do path finding, etc.
     */
    class NavMesh {
        public:
            NavMesh();
            NavMesh(const NavMesh&);
            NavMesh& operator=(const NavMesh&) = delete;
            ~NavMesh() = default;

            unsigned int getUpdateId() const;

            void copyAllPolygons(const std::vector<std::shared_ptr<NavPolygon>>&);
            const std::vector<std::shared_ptr<NavPolygon>>& getPolygons() const;

            void svgMeshExport(std::string) const;

        private:
            unsigned int changeUpdateId();

            static unsigned int nextUpdateId;
            unsigned int updateId;

            std::vector<std::shared_ptr<NavPolygon>> polygons;
    };

}
