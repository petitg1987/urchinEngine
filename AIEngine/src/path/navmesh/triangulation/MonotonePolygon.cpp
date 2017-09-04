#include "MonotonePolygon.h"

namespace urchin
{

    MonotonePolygon::MonotonePolygon(const std::vector<unsigned int> &ccwPoints) :
        ccwPoints(ccwPoints)
    {

    }

    const std::vector<unsigned int> &MonotonePolygon::getCcwPoints() const
    {
        return ccwPoints;
    }

    void MonotonePolygon::addSharedEdge(unsigned int edgeStartIndex, unsigned int edgeEndIndex)
    {
        sharedEdges.insert(computeEdgeId(edgeStartIndex, edgeEndIndex));
    }

    bool MonotonePolygon::isSharedEdge(unsigned int edgeStartIndex, unsigned int edgeEndIndex) const
    {
        return sharedEdges.find(computeEdgeId(edgeStartIndex, edgeEndIndex))!=sharedEdges.end();
    }

    uint_fast64_t MonotonePolygon::computeEdgeId(unsigned int edgeStartIndex, unsigned int edgeEndIndex) const
    {
        auto edgeId = static_cast<uint_fast64_t>(std::min(edgeStartIndex, edgeEndIndex));
        edgeId = edgeId << 32;
        return edgeId + std::max(edgeStartIndex, edgeEndIndex);
    }

}
