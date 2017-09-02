#include "MonotonePolygon.h"

namespace urchin
{

    MonotonePolygonEdge::MonotonePolygonEdge(unsigned int monotonePolygonIndex, unsigned int edgeId) :
        monotonePolygonIndex(monotonePolygonIndex),
        edgeId(edgeId)
    {

    }

    MonotonePolygon::MonotonePolygon(const std::vector<unsigned int> &ccwPoints) :
        ccwPoints(ccwPoints)
    {

    }

    const std::vector<unsigned int> &MonotonePolygon::getCcwPoints() const
    {
        return ccwPoints;
    }

    void MonotonePolygon::addConnection(unsigned int edgeId, MonotonePolygonEdge monotonePolygonEdge)
    {
        connections.insert(std::make_pair(edgeId, monotonePolygonEdge));
    }

    const std::map<unsigned int, MonotonePolygonEdge> &MonotonePolygon::getConnections() const
    {
        return connections;
    }

}
