#ifndef URCHINENGINE_NAVLINK_H
#define URCHINENGINE_NAVLINK_H

#include <memory>

#include "path/navmesh/model/output/NavJumpConstraint.h"

namespace urchin
{

    class NavPolygon;
    class NavTriangle;

    enum NavLinkType
    {
        DIRECT,
        JUMP
    };

    /**
     * Represent a link between two triangles.
     * Source triangle is the one which contains the NavLink class and the target polygon/triangle is defined in NavLink class.
     */
    class NavLink
    {
        public:
            ~NavLink();
            static std::shared_ptr<NavLink> newDirectLink(unsigned int, const std::shared_ptr<NavTriangle> &);
            static std::shared_ptr<NavLink> newJumpLink(unsigned int, const std::shared_ptr<NavTriangle> &, NavJumpConstraint *);

            std::shared_ptr<NavLink> copyLink(const std::shared_ptr<NavTriangle> &) const;

            NavLinkType getLinkType() const;
            unsigned int getSourceEdgeIndex() const;
            const std::shared_ptr<NavTriangle> &getTargetTriangle() const;

            const NavJumpConstraint *getJumpConstraint() const;

        private:
            NavLink(NavLinkType, unsigned int, std::shared_ptr<NavTriangle>, NavJumpConstraint *);

            NavLinkType linkType;
            unsigned int sourceEdgeIndex;
            std::shared_ptr<NavTriangle> targetTriangle;

            NavJumpConstraint *jumpConstraint;
    };

}

#endif
