#pragma once

#include <vector>

#include <path/navmesh/csg/CSGPolygon.h>

namespace urchin {

    template<class T> class PolygonsSubtraction {
        public:
            static PolygonsSubtraction<T>& instance();

            const std::vector<CSGPolygon<T>>& subtractPolygons(const CSGPolygon<T>&, const CSGPolygon<T>&) const;
            const std::vector<CSGPolygon<T>>& subtractPolygons(const CSGPolygon<T>&, const CSGPolygon<T>&, bool&) const;

        private:
            PolygonsSubtraction() = default;

            mutable std::vector<CSGPolygon<T>> subtractedPolygons;
    };

}
