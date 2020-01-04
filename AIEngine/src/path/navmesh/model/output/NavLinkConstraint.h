#ifndef URCHINENGINE_NAVLINKCONSTRAINT_H
#define URCHINENGINE_NAVLINKCONSTRAINT_H

#include "UrchinCommon.h"

namespace urchin
{

    class NavLinkConstraint
    {
        public:
            NavLinkConstraint(float, float, float);
            NavLinkConstraint(const NavLinkConstraint &) = default;

            float getSourceEdgeLinkStartRange() const;
            float getSourceEdgeLinkEndRange() const;
            LineSegment3D<float> computeSourceJumpEdge(const LineSegment3D<float> &) const;

            float getTargetEdgeIndex() const;

        private:
            float sourceEdgeLinkStartRange;
            float sourceEdgeLinkEndRange;

            float targetEdgeIndex;
    };

}

#endif
