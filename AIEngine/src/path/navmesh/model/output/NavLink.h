#ifndef URCHINENGINE_NAVLINK_H
#define URCHINENGINE_NAVLINK_H

#include <memory>

namespace urchin
{

    class NavPolygon;
    class NavTriangle;

    struct JumpConstraint
    {
        float sourceEdgeStartPoint;
        float sourceEdgeEndPoint;

        float targetEdgeIndex;
        float targetEdgeStartPoint;
        float targetEdgeEndPoint;
    };

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
            static std::shared_ptr<NavLink> newJumpLink(unsigned int, const std::shared_ptr<NavTriangle> &, JumpConstraint *);

            NavLinkType getLinkType() const;
            unsigned int getSourceEdgeIndex() const;
            const std::shared_ptr<NavTriangle> &getTargetTriangle() const;

            const JumpConstraint *getJumpConstraint() const;

        private:
            NavLink(NavLinkType, unsigned int, std::shared_ptr<NavTriangle> , JumpConstraint *);

            NavLinkType linkType;
            unsigned int sourceEdgeIndex;
            std::shared_ptr<NavTriangle> targetTriangle;

            JumpConstraint *jumpConstraint;
    };

}

#endif
