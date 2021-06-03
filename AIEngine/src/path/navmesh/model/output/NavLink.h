#ifndef URCHINENGINE_NAVLINK_H
#define URCHINENGINE_NAVLINK_H

#include <memory>

#include <path/navmesh/model/output/NavLinkConstraint.h>

namespace urchin {

    class NavTriangle;

    enum NavLinkType {
        STANDARD, //link between two triangles of the same polygon
        JOIN_POLYGONS, //link between two triangles of different polygons
        JUMP //jump between two triangles
    };

    /**
     * Represent a link between two triangles.
     * Source triangle is the one which contains the NavLink class and the target polygon/triangle is defined in NavLink class.
     */
    class NavLink {
        public:
            ~NavLink();
            static std::shared_ptr<NavLink> newStandardLink(std::size_t, const std::shared_ptr<NavTriangle>&);
            static std::shared_ptr<NavLink> newJoinPolygonsLink(std::size_t, const std::shared_ptr<NavTriangle>&, NavLinkConstraint*);
            static std::shared_ptr<NavLink> newJumpLink(std::size_t, const std::shared_ptr<NavTriangle>&, NavLinkConstraint*);

            std::shared_ptr<NavLink> copyLink(const std::shared_ptr<NavTriangle>&) const;

            NavLinkType getLinkType() const;
            std::size_t getSourceEdgeIndex() const;
            std::shared_ptr<NavTriangle> getTargetTriangle() const;

            const NavLinkConstraint* getLinkConstraint() const;

        private:
            NavLink(NavLinkType, std::size_t, const std::shared_ptr<NavTriangle>&, NavLinkConstraint*);

            NavLinkType linkType;
            std::size_t sourceEdgeIndex;
            std::weak_ptr<NavTriangle> targetTriangle; //use weak_ptr to avoid cyclic references (=memory leak) between two triangles

            NavLinkConstraint* linkConstraint;
    };

}

#endif
