#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <path/navmesh/model/output/NavLink.h>
#include <path/navmesh/model/output/NavLinkConstraint.h>

namespace urchin {

    class NavPolygon;

    class NavTriangle {
        public:
            NavTriangle(std::size_t, std::size_t, std::size_t);
            NavTriangle(const NavTriangle&);

            void attachNavPolygon(const std::shared_ptr<NavPolygon>&);

            std::shared_ptr<NavPolygon> getNavPolygon() const;
            const Point3<float>& getCenterPoint() const;

            const std::size_t* getIndices() const;
            std::size_t getIndex(std::size_t) const;

            void addStandardLink(std::size_t, const std::shared_ptr<NavTriangle>&);
            void addJoinPolygonsLink(std::size_t, const std::shared_ptr<NavTriangle>&, NavLinkConstraint*);
            void addJumpLink(std::size_t, const std::shared_ptr<NavTriangle>&, NavLinkConstraint*);
            void addLink(const std::shared_ptr<NavLink>&);
            void removeLinksTo(const std::shared_ptr<NavPolygon>&);
            std::vector<std::shared_ptr<NavLink>> getLinks() const;

            bool hasEdgeLinks(std::size_t) const;
            bool isExternalEdge(std::size_t) const;
            LineSegment3D<float> computeEdge(std::size_t) const;

        private:
            void assertLinksValidity() const;

            std::weak_ptr<NavPolygon> navPolygon; //use weak_ptr to avoid cyclic references (=memory leak) between triangle and polygon

            std::size_t indices[3];
            std::vector<std::shared_ptr<NavLink>> links;

            Point3<float> centerPoint;
    };

}
