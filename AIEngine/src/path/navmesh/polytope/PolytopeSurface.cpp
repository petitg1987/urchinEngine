#include "PolytopeSurface.h"

namespace urchin
{
    PolytopeSurface::PolytopeSurface(bool expandedSurface) :
            expandedSurface(expandedSurface),
            walkableCandidate(true)
    {

    }

    bool PolytopeSurface::isExpandedSurface() const
    {
        return expandedSurface;
    }

    void PolytopeSurface::setWalkableCandidate(bool walkableCandidate)
    {
        this->walkableCandidate = walkableCandidate;
    }

    bool PolytopeSurface::isWalkableCandidate() const
    {
        return walkableCandidate;
    }
}
