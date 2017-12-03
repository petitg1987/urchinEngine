#ifndef URCHINENGINE_POLYTOPETERRAINSURFACE_H
#define URCHINENGINE_POLYTOPETERRAINSURFACE_H

#include "path/navmesh/polytope/PolytopeSurface.h"

namespace urchin
{

    class PolytopeTerrainSurface : public PolytopeSurface
    {
        public:
            bool isWalkable(float) const override;
            std::vector<Point2<float>> getOutlineCwPoints() const override;
            Plane<float> getPlaneIn(const Rectangle<float> &) const override;
            Point3<float> elevatePoint(const Point2<float> &, const NavMeshAgent &) const override;
    };

}

#endif
