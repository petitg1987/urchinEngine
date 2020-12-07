#ifndef URCHINENGINE_NAVLINKCONSTRAINT_H
#define URCHINENGINE_NAVLINKCONSTRAINT_H

#include "UrchinCommon.h"

namespace urchin {

    class NavLinkConstraint {
        public:
            NavLinkConstraint(float, float, std::size_t);
            NavLinkConstraint(const NavLinkConstraint&) = default;

            float getSourceEdgeLinkStartRange() const;
            float getSourceEdgeLinkEndRange() const;
            LineSegment3D<float> computeSourceJumpEdge(const LineSegment3D<float>&) const;

            std::size_t getTargetEdgeIndex() const;

        private:
            float sourceEdgeLinkStartRange;
            float sourceEdgeLinkEndRange;

            std::size_t targetEdgeIndex;
    };

}

#endif
