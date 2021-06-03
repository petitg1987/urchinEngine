#ifndef URCHINENGINE_CSGPOLYGONPATH_H
#define URCHINENGINE_CSGPOLYGONPATH_H

#include <path/navmesh/csg/CSGPolygon.h>
#include <UrchinCommon.h>

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

#endif
