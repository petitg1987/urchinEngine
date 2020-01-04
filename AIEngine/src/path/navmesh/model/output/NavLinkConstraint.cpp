#include <cassert>

#include "NavLinkConstraint.h"

namespace urchin
{
    NavLinkConstraint::NavLinkConstraint(float sourceEdgeLinkStartRange, float sourceEdgeLinkEndRange, float targetEdgeIndex) :
            sourceEdgeLinkStartRange(sourceEdgeLinkStartRange),
            sourceEdgeLinkEndRange(sourceEdgeLinkEndRange),
            targetEdgeIndex(targetEdgeIndex)
    {
        assert(targetEdgeIndex <= 2);
        assert(sourceEdgeLinkStartRange >= sourceEdgeLinkEndRange);
    }

    float NavLinkConstraint::getSourceEdgeLinkStartRange() const
    {
        return sourceEdgeLinkStartRange;
    }

    float NavLinkConstraint::getSourceEdgeLinkEndRange() const
    {
        return sourceEdgeLinkEndRange;
    }

    LineSegment3D<float> NavLinkConstraint::computeSourceJumpEdge(const LineSegment3D<float> &sourceEdge) const
    {
        return LineSegment3D<float>(
                sourceEdgeLinkStartRange * sourceEdge.getA() + (1.0f - sourceEdgeLinkStartRange) * sourceEdge.getB(),
                sourceEdgeLinkEndRange * sourceEdge.getA() + (1.0f - sourceEdgeLinkEndRange) * sourceEdge.getB());
    }

    float NavLinkConstraint::getTargetEdgeIndex() const
    {
        return targetEdgeIndex;
    }
}
