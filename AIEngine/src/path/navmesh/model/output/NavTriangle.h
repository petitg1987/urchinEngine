#ifndef URCHINENGINE_NAVTRIANGLE_H
#define URCHINENGINE_NAVTRIANGLE_H

#include <vector>
#include "UrchinCommon.h"

#include "path/navmesh/model/output/NavLink.h"
#include "path/navmesh/model/output/NavJumpConstraint.h"

namespace urchin
{

    class NavPolygon;

    class NavTriangle
    {
        public:
            NavTriangle(std::size_t, std::size_t, std::size_t);

            void attachNavPolygon(const std::shared_ptr<NavPolygon> &);

            std::shared_ptr<NavPolygon> getNavPolygon() const;
            const Point3<float> &getCenterPoint() const;

            const std::size_t *getIndices() const;
            std::size_t getIndex(std::size_t) const;

            void addDirectLink(std::size_t, const std::shared_ptr<NavTriangle> &);
            void addJumpLink(std::size_t, const std::shared_ptr<NavTriangle> &, NavJumpConstraint *);
            std::vector<std::shared_ptr<NavLink>> getLinks() const;

            bool hasEdgeLinks(std::size_t edgeIndex) const;
            LineSegment3D<float> computeEdge(std::size_t) const;

        private:
            std::weak_ptr<NavPolygon> navPolygon; //use weak_ptr to avoid cyclic references between triangle and polygon

            std::size_t indices[3];
            std::vector<std::shared_ptr<NavLink>> links;

            Point3<float> centerPoint;
    };

}

#endif
