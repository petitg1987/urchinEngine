#include "PolytopeSurface.h"

namespace urchin
{
    PolytopeSurface::PolytopeSurface() :
            walkableCandidate(true)
    {

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
