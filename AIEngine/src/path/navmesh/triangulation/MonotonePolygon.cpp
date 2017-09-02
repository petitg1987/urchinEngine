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

    void MonotonePolygon::addSharedEdge(uint_fast64_t edgeId, unsigned int monotonePolygonIndex)
    {
        sharedEdges.insert(std::make_pair(edgeId, monotonePolygonIndex));
    }

    const std::map<uint_fast64_t, unsigned int> &MonotonePolygon::getSharedEdges() const
    {
        return sharedEdges;
    }

}
