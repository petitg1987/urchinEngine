#include "PolytopeTerrainSurface.h"

namespace urchin
{
    bool PolytopeTerrainSurface::isWalkable(float) const
    {
        return isWalkableCandidate();
    }

    std::vector<Point2<float>> PolytopeTerrainSurface::getOutlineCwPoints() const
    {
        std::vector<Point2<float>> flatPoints;

        //TODO return 4 point of terrain...

        return flatPoints;
    }
}
