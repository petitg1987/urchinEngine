#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <path/navmesh/csg/CSGPolygon.h>
#include <path/navmesh/csg/CSGPolygonPath.h>

namespace urchin {

    template<class T> class PolygonsUnion {
        public:
            static PolygonsUnion<T>& instance();

            std::vector<CSGPolygon<T>>& unionPolygons(const std::vector<CSGPolygon<T>>&) const;

        private:
            PolygonsUnion() = default;

            const std::vector<CSGPolygonPath>& unionTwoPolygonPaths(const CSGPolygonPath&, const CSGPolygonPath&) const;

            void logInputData(const std::vector<CSGPolygon<T>>&, std::string_view, Logger::CriticalityLevel) const;

            mutable std::vector<CSGPolygon<T>> mergedPolygons;
            mutable std::vector<CSGPolygonPath> allPolygonPaths;
            mutable std::vector<CSGPolygonPath> twoPolygonUnions;
    };

}
