#include <stdexcept>
#include <stack>
#include <algorithm>

#include <path/navmesh/triangulation/TriangulationAlgorithm.h>

namespace mapbox::util {

    auto nth<0, urchin::Point2<float>>::get(const urchin::Point2<float> &point) {
        return point.X;
    }

    auto nth<1, urchin::Point2<float>>::get(const urchin::Point2<float> &point) {
        return point.Y;
    }

}

namespace urchin {

    /**
     * @param polygonPoints Polygon points in the first index and then holes points. Any winding order works.
     */
    TriangulationAlgorithm::TriangulationAlgorithm(std::vector<std::vector<Point2<float>>>&& polygonPoints) :
            polygonPoints(std::move(polygonPoints)) {

    }

    const std::vector<std::shared_ptr<NavTriangle>>& TriangulationAlgorithm::triangulate() {
        std::vector<uint32_t> ccwIndices = mapbox::earcut<uint32_t>(polygonPoints);
        assert(ccwIndices.size() % 3 == 0);

        navTriangles.reserve(ccwIndices.size() / 3);

        std::map<uint_fast64_t, std::vector<std::shared_ptr<NavTriangle>>> edgeToTriangles;
        for (unsigned int i = 0; i < ccwIndices.size(); i += 3) {
            uint32_t index1 = ccwIndices[i];
            uint32_t index2 = ccwIndices[i + 1];
            uint32_t index3 = ccwIndices[i + 2];

            auto navTriangle = std::make_shared<NavTriangle>(index1, index2, index3);
            navTriangles.emplace_back(navTriangle);

            uint_fast64_t edgeId1 = computeEdgeId(index1, index2);
            uint_fast64_t edgeId2 = computeEdgeId(index2, index3);
            uint_fast64_t edgeId3 = computeEdgeId(index3, index1);

            edgeToTriangles[edgeId1].emplace_back(navTriangle);
            edgeToTriangles[edgeId2].emplace_back(navTriangle);
            edgeToTriangles[edgeId3].emplace_back(navTriangle);
        }

        linkTriangles(edgeToTriangles);

        return navTriangles;
    }

    uint_fast64_t TriangulationAlgorithm::computeEdgeId(std::size_t edgeStartIndex, std::size_t edgeEndIndex) const {
        auto edgeId = (uint_fast64_t)std::min(edgeStartIndex, edgeEndIndex);
        edgeId = edgeId << 32u;
        return edgeId + std::max(edgeStartIndex, edgeEndIndex);
    }

    void TriangulationAlgorithm::linkTriangles(const std::map<uint_fast64_t, std::vector<std::shared_ptr<NavTriangle>>>& edgeToTriangles) {
        for (const auto& navTriangle : navTriangles) {
            for (unsigned int i = 0; i < 3; ++i) {
                std::size_t edgeIndex1 = i;
                std::size_t edgeIndex2 = (i + 1) % 3;
                uint_fast64_t edgeId = computeEdgeId(navTriangle->getIndex(edgeIndex1), navTriangle->getIndex(edgeIndex2));

                for (auto &neighborTriangle : edgeToTriangles.find(edgeId)->second) {
                    if (neighborTriangle != navTriangle) {
                        navTriangle->addLink(NavLink::newStandardLink(i, neighborTriangle));
                    }
                }
            }
        }
    }

}
