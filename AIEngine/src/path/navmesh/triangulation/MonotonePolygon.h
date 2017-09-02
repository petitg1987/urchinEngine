#ifndef URCHINENGINE_MONOTONEPOLYGON_H
#define URCHINENGINE_MONOTONEPOLYGON_H

#include <vector>
#include <map>

namespace urchin
{

    class MonotonePolygon
    {
        public:
            explicit MonotonePolygon(const std::vector<unsigned int> &);

            const std::vector<unsigned int> &getCcwPoints() const;

            void addSharedEdge(uint_fast64_t, unsigned int);
            const std::map<uint_fast64_t, unsigned int> &getSharedEdges() const;

        private:
            std::vector<unsigned int> ccwPoints;
            std::map<uint_fast64_t, unsigned int> sharedEdges; //first: edge id, second: other monotone polygon index
    };

}

#endif
