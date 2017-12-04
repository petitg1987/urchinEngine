#ifndef URCHINENGINE_POLYTOPETERRAINSURFACE_H
#define URCHINENGINE_POLYTOPETERRAINSURFACE_H

#include "path/navmesh/polytope/PolytopeSurface.h"

namespace urchin
{

    class PolytopeTerrainSurface : public PolytopeSurface
    { //TODO eliminate un-walkable terrain part
        public:
            PolytopeTerrainSurface(const Point3<float> &, const std::vector<Point3<float>> &, unsigned int, unsigned int);

            bool isWalkable(float) const override;
            Rectangle<float> computeXZRectangle() const override;

            std::vector<Point2<float>> getOutlineCwPoints() const override;
            Plane<float> getPlaneIn(const Rectangle<float> &) const override;
            Point3<float> elevatePoint(const Point2<float> &, const NavMeshAgent &) const override;

            const Point3<float> &getPosition() const;
            const std::vector<Point3<float>> getLocalVertices() const;
            unsigned int getXLength() const;
            unsigned int getZLength() const;

        private:
            void buildOutlineCwPoints();
            Point3<float> retrieveGlobalVertex(const Point2<float> &) const;

            Point3<float> position;
            std::vector<Point3<float>> localVertices;
            unsigned int xLength;
            unsigned int zLength;

            std::vector<Point2<float>> outlineCwPoints;
    };

}

#endif
