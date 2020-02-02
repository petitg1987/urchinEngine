#ifndef URCHINENGINE_POLYTOPEBUILDER_H
#define URCHINENGINE_POLYTOPEBUILDER_H

#include <iostream>
#include <memory>
#include "UrchinCommon.h"

#include "input/AIObject.h"
#include "input/AITerrain.h"
#include "path/navmesh/polytope/Polytope.h"
#include "path/navmesh/polytope/PolytopeSurface.h"
#include "path/navmesh/polytope/services/TerrainSplitService.h"
#include "path/navmesh/polytope//services/PlaneSurfaceSplitService.h"
#include "path/navmesh/model/output/NavMeshAgent.h"

namespace urchin
{

    class PolytopeBuilder : public Singleton<PolytopeBuilder>
    {
        public:
            friend class Singleton<PolytopeBuilder>;

            std::vector<std::unique_ptr<Polytope>> buildExpandedPolytopes(const std::shared_ptr<AIObject> &, const std::shared_ptr<NavMeshAgent> &);
            std::vector<std::unique_ptr<Polytope>> buildExpandedPolytope(const std::shared_ptr<AITerrain> &, const std::shared_ptr<NavMeshAgent> &);

        private:
            PolytopeBuilder();
            ~PolytopeBuilder() override;

            std::unique_ptr<Polytope> createExpandedPolytopeFor(const std::string &, OBBox<float> *, const std::shared_ptr<NavMeshAgent> &) const;
            std::unique_ptr<Polytope> createExpandedPolytopeFor(const std::string &, Capsule<float> *, const std::shared_ptr<NavMeshAgent> &) const;
            std::unique_ptr<Polytope> createExpandedPolytopeFor(const std::string &, Cone<float> *, const std::shared_ptr<NavMeshAgent> &) const;
            std::unique_ptr<Polytope> createExpandedPolytopeFor(const std::string &, ConvexHull3D<float> *, const std::shared_ptr<NavMeshAgent> &) const;
            std::unique_ptr<Polytope> createExpandedPolytopeFor(const std::string &, Cylinder<float> *, const std::shared_ptr<NavMeshAgent> &) const;
            std::unique_ptr<Polytope> createExpandedPolytopeFor(const std::string &, Sphere<float> *, const std::shared_ptr<NavMeshAgent> &) const;

            std::vector<Point3<float>> createExpandedPoints(const std::vector<Point3<float>> &, const std::shared_ptr<NavMeshAgent> &) const;
            std::vector<Plane<float>> createExpandedBoxPlanes(const std::vector<Point3<float>> &, const std::shared_ptr<NavMeshAgent> &) const;
            Plane<float> createExpandedPlane(const Point3<float> &, const Point3<float> &, const Point3<float> &, const std::shared_ptr<NavMeshAgent> &) const;
            std::vector<Point3<float>> expandBoxPoints(const std::vector<Plane<float>> &) const;

            std::vector<std::shared_ptr<PolytopeSurface>> createExpandedPolytopeSurfaces(const std::vector<Point3<float>> &,
                    const std::vector<Point3<float>> &, const std::shared_ptr<NavMeshAgent> &) const;

            static const unsigned int POINT_INDEX_TO_PLANES[][4];
            static const unsigned int PLANE_INDEX_TO_POINTS[][3];

            PlaneSurfaceSplitService *planeSurfaceSplitService;
            TerrainSplitService *terrainSplitService;
    };

}

#endif
