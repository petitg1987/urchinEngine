#ifndef URCHINENGINE_POLYTOPETERRAINSURFACE_H
#define URCHINENGINE_POLYTOPETERRAINSURFACE_H

#include <memory>

#include "UrchinCommon.h"
#include "path/navmesh/polytope/PolytopeSurface.h"

namespace urchin
{

    class PolytopeTerrainSurface : public PolytopeSurface
    {
        public:
            PolytopeTerrainSurface(const Point3<float> &, const std::vector<Point3<float>> &, unsigned int, unsigned int, const std::vector<CSGPolygon<float>> &);

            bool isWalkable(float) const override;
            Rectangle<float> computeXZRectangle() const override;

            const std::vector<Point2<float>> &getOutlineCwPoints() const override;
            Plane<float> getPlane(const Rectangle<float> &, const NavMeshAgent &) const override;
            const std::vector<CSGPolygon<float>> &getSelfObstacles() const override;
            Point3<float> computeRealPoint(const Point2<float> &, const NavMeshAgent &) const override;
            NavTopography *newNavTopography() const override;

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
            std::shared_ptr<HeightfieldPointHelper<float>> heightfieldPointHelper;

            std::vector<Point2<float>> outlineCwPoints;
            std::vector<CSGPolygon<float>> selfObstacles;
    };

}

#endif
