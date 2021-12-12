#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <path/navmesh/csg/CSGPolygon.h>
#include <path/navmesh/csg/CSGPolygonPath.h>

namespace urchin {

    template<class T> class PolygonsUnion : public Singleton<PolygonsUnion<T>> {
        public:
            friend class Singleton<PolygonsUnion<T>>;

            std::vector<CSGPolygon<T>>& unionPolygons(const std::vector<CSGPolygon<T>>&) const;

        private:
            PolygonsUnion() = default;

            const std::vector<CSGPolygonPath>& unionTwoPolygonPaths(const CSGPolygonPath&, const CSGPolygonPath&) const;

            void logInputData(const std::vector<CSGPolygon<T>>&, const std::string&, Logger::CriticalityLevel) const;

            mutable std::vector<CSGPolygon<T>> mergedPolygons;
            mutable std::vector<CSGPolygonPath> allPolygonPaths;
            mutable std::vector<CSGPolygonPath> twoPolygonUnions;
    };

}
