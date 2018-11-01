#ifndef URCHINENGINE_NAVTRIANGLE_H
#define URCHINENGINE_NAVTRIANGLE_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

    class NavPolygon;

    class NavTriangle
    {
        public:
            NavTriangle(unsigned int, unsigned int, unsigned int);

            void attachNavPolygon(const std::shared_ptr<NavPolygon> &);

            std::shared_ptr<NavPolygon> getNavPolygon() const;
            const Point3<float> &getCenterPoint() const;

            const unsigned int *getIndices() const;
            unsigned int getIndex(unsigned int) const;

            void addNeighbor(unsigned int, const std::shared_ptr<NavTriangle> &);
            const std::shared_ptr<NavTriangle> &getNeighbor(unsigned int) const;

            LineSegment3D<float> computeEdge(unsigned int) const;

        private:
            std::weak_ptr<NavPolygon> navPolygon; //use weak_ptr to avoid cyclic references between triangle and polygon

            unsigned int indices[3];
            std::shared_ptr<NavTriangle> neighbors[3];

            Point3<float> centerPoint;
    };

}

#endif
