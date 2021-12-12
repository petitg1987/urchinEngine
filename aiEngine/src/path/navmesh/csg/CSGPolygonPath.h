#pragma once

#include <libs/clipper/clipper.h>
#include <path/navmesh/csg/CSGPolygon.h>

namespace urchin {

    class CSGPolygonPath {
        public:
            CSGPolygonPath(ClipperLib::Path, std::string);
            template<class T> explicit CSGPolygonPath(const CSGPolygon<T>&);

            template<class T> CSGPolygon<T> toCSGPolygon() const;

            const ClipperLib::Path& getPath() const;
            const std::string& getName() const;

        private:
            ClipperLib::Path path;
            std::string name;
    };

}
