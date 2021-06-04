#ifndef URCHINENGINE_TRIANGULATIONALGORITHM_H
#define URCHINENGINE_TRIANGULATIONALGORITHM_H

#include <vector>
#include <map>
#include <UrchinCommon.h>

#include <path/navmesh/model/output/NavTriangle.h>

namespace mapbox::util {

    template<> struct nth<0, urchin::Point2<float>> {
        static auto get(const urchin::Point2<float> &t);
    };

    template<> struct nth<1, urchin::Point2<float>> {
        static auto get(const urchin::Point2<float> &t);
    };

}

namespace urchin {

    class TriangulationAlgorithm {
        public:
            explicit TriangulationAlgorithm(std::vector<std::vector<Point2<float>>>&&);

            const std::vector<std::shared_ptr<NavTriangle>>& triangulate();

        private:
            uint_fast64_t computeEdgeId(std::size_t, std::size_t) const;
            void linkTriangles(const std::map<uint_fast64_t, std::vector<std::shared_ptr<NavTriangle>>>&);

            std::vector<std::vector<Point2<float>>> polygonPoints;
            std::vector<std::shared_ptr<NavTriangle>> navTriangles;
    };

}

#endif
