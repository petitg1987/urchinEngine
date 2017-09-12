#ifndef URCHINENGINE_NAVTRIANGLE_H
#define URCHINENGINE_NAVTRIANGLE_H

#include <vector>
#include "UrchinCommon.h"

#include "path/navmesh/model/ref/NavEdgeRef.h"

namespace urchin
{

    class NavTriangle
    {
        public:
            NavTriangle(unsigned int, unsigned int, unsigned int);

            void computeCenterPoint(const std::vector<Point3<float>> &);
            const Point3<float> &getCenterPoint() const;

            const unsigned int *getIndices() const;
            unsigned int getIndex(unsigned int) const;

            void addNeighbor(unsigned int, int);
            const int *getNeighbors() const;
            int getNeighbor(unsigned int) const;

            const std::vector<NavEdgeRef> &getLinks() const;

        private:
            unsigned int indices[3];
            int neighbors[3];

            Point3<float> centerPoint;

            std::vector<NavEdgeRef> links;
    };

}

#endif
