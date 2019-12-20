#ifndef URCHINENGINE_NAVJUMPCONSTRAINT_H
#define URCHINENGINE_NAVJUMPCONSTRAINT_H

#include "UrchinCommon.h"

namespace urchin
{

    class NavJumpConstraint
    {
        public:
            NavJumpConstraint(float, float, float);

            float getSourceEdgeJumpStartRange() const;
            float getSourceEdgeJumpEndRange() const;
            LineSegment3D<float> computeSourceJumpEdge(const LineSegment3D<float> &) const;

            float getTargetEdgeIndex() const;

        private:
            float sourceEdgeJumpStartRange;
            float sourceEdgeJumpEndRange;

            float targetEdgeIndex;
    };

}

#endif
