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
            static std::shared_ptr<NavLink> newDirectLink(unsigned int, const std::shared_ptr<NavPolygon> &, const std::shared_ptr<NavTriangle> &);
            static std::shared_ptr<NavLink> newJumpLink(unsigned int, const std::shared_ptr<NavPolygon> &, const std::shared_ptr<NavTriangle> &, JumpConstraint *);

            NavLinkType getLinkType() const;
            const JumpConstraint *getJumpConstraint() const;

            unsigned int getSourceEdgeIndex() const;

            std::shared_ptr<NavPolygon> getTargetPolygon() const;
            std::shared_ptr<NavTriangle> getTargetTriangle() const;

        private:
            NavLink(NavLinkType, unsigned int, const std::shared_ptr<NavPolygon> &, const std::shared_ptr<NavTriangle> &, JumpConstraint *);

            NavLinkType linkType;
            unsigned int sourceEdgeIndex;
            std::weak_ptr<NavPolygon> targetPolygon;
            std::weak_ptr<NavTriangle> targetTriangle; //use weak_ptr to avoid cyclic references between triangles //TODO review comment & weak_ptr ?

            JumpConstraint *jumpConstraint;
    };

}

#endif
