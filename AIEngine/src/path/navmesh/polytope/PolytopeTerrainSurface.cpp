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

        //TODO return 4 points of terrain in CW

        return flatPoints;
    }

    Plane<float> PolytopeTerrainSurface::getPlaneIn(const Rectangle<float> &box) const
    {
        Plane<float> plane;

        //TODO create plane + check if walkable ?

        return plane;
    }

    Point3<float> PolytopeTerrainSurface::elevatePoint(const Point2<float> &point, const NavMeshAgent &navMeshAgent) const
    {
        Point3<float> elevatedPoint;

        //TODO compute elevatePoint

        return elevatedPoint;
    }
}
