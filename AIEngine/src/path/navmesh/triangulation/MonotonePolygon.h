#ifndef URCHINENGINE_MONOTONEPOLYGON_H
#define URCHINENGINE_MONOTONEPOLYGON_H

#include <vector>
#include <map>

namespace urchin
{

    struct MonotonePolygonEdge
    {
        MonotonePolygonEdge(unsigned int, unsigned int);

        unsigned int monotonePolygonIndex; //ref. to another monotone polygon
        unsigned int edgeId; //TODO use unsigned int ?!
    };

    class MonotonePolygon
    {
        public:
            explicit MonotonePolygon(const std::vector<unsigned int> &);

            const std::vector<unsigned int> &getCcwPoints() const;

            void addConnection(unsigned int, MonotonePolygonEdge);
            const std::map<unsigned int, MonotonePolygonEdge> &getConnections() const;

        private:
            std::vector<unsigned int> ccwPoints;
            std::map<unsigned int, MonotonePolygonEdge> connections; //first: edge id on this monotone, second: edge on another monotone polygon //TODO use unsigned int ?!
    };

}

#endif
