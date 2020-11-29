#include "MonotonePolygon.h"

namespace urchin {

    void MonotonePolygon::setCcwPoints(const std::vector<std::size_t>& ccwPoints) {
        this->ccwPoints = ccwPoints;
    }

    const std::vector<std::size_t>& MonotonePolygon::getCcwPoints() const {
        return ccwPoints;
    }

    void MonotonePolygon::addSharedEdge(unsigned int edgeStartIndex, unsigned int edgeEndIndex) {
        sharedEdges.insert(computeEdgeId(edgeStartIndex, edgeEndIndex));
    }

    const std::set<uint_fast64_t>& MonotonePolygon::getSharedEdges() const {
        return sharedEdges;
    }

    bool MonotonePolygon::isSharedEdge(unsigned int edgeStartIndex, unsigned int edgeEndIndex) const {
        return sharedEdges.find(computeEdgeId(edgeStartIndex, edgeEndIndex))!=sharedEdges.end();
    }

    uint_fast64_t MonotonePolygon::computeEdgeId(unsigned int edgeStartIndex, unsigned int edgeEndIndex) const {
        auto edgeId = static_cast<uint_fast64_t>(std::min(edgeStartIndex, edgeEndIndex));
        edgeId = edgeId << 32u;
        return edgeId + std::max(edgeStartIndex, edgeEndIndex);
    }

}
