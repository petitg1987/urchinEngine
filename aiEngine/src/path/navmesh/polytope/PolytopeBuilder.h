#pragma once

#include <iostream>
#include <memory>
#include <UrchinCommon.h>

#include <input/AIObject.h>
#include <input/AITerrain.h>
#include <path/navmesh/polytope/Polytope.h>
#include <path/navmesh/polytope/PolytopeSurface.h>
#include <path/navmesh/polytope/services/TerrainSplitService.h>
#include <path/navmesh/polytope//services/PlaneSurfaceSplitService.h>
#include <path/navmesh/model/output/NavMeshAgent.h>

namespace urchin {

    class PolytopeBuilder : public Singleton<PolytopeBuilder> {
        public:
            friend class Singleton<PolytopeBuilder>;

            std::vector<std::unique_ptr<Polytope>> buildExpandedPolytopes(const AIObject&, const NavMeshAgent&) const;
            std::vector<std::unique_ptr<Polytope>> buildExpandedPolytope(const AITerrain&, const NavMeshAgent&) const;

        private:
            PolytopeBuilder();

            std::unique_ptr<Polytope> createExpandedPolytope(std::string, const OBBox<float>*, const NavMeshAgent&) const;
            std::unique_ptr<Polytope> createExpandedPolytope(std::string, const Capsule<float>*, const NavMeshAgent&) const;
            std::unique_ptr<Polytope> createExpandedPolytope(std::string, const Cone<float>*, const NavMeshAgent&) const;
            std::unique_ptr<Polytope> createExpandedPolytope(std::string, const ConvexHull3D<float>*, const NavMeshAgent&) const;
            std::unique_ptr<Polytope> createExpandedPolytope(std::string, const Cylinder<float>*, const NavMeshAgent&) const;
            std::unique_ptr<Polytope> createExpandedPolytope(std::string, const Sphere<float>*, const NavMeshAgent&) const;

            std::vector<Point3<float>> createExpandedPoints(const std::vector<Point3<float>>&, const NavMeshAgent&) const;
            std::vector<Plane<float>> createExpandedBoxPlanes(const std::vector<Point3<float>>&, const NavMeshAgent&) const;
            Plane<float> createExpandedPlane(const Point3<float>&, const Point3<float>&, const Point3<float>&, const NavMeshAgent&) const;
            std::vector<Point3<float>> expandBoxPoints(const std::vector<Plane<float>>&) const;

            std::vector<std::shared_ptr<PolytopeSurface>> createExpandedPolytopeSurfaces(const std::vector<Point3<float>>&,
                    const std::vector<Point3<float>>&, const NavMeshAgent&) const;

            static constexpr std::array<std::array<unsigned int, 4>, 6> POINT_INDEX_TO_PLANES = {{
                {0, 2, 3, 1}, //right
                {4, 5, 7, 6}, //left
                {0, 1, 5, 4}, //top
                {3, 2, 6, 7}, //bottom
                {0, 4, 6, 2}, //front
                {1, 3, 7, 5} //back
            }};
            static constexpr std::array<std::array<unsigned int, 3>, 8> PLANE_INDEX_TO_POINTS = {{
                {0, 2, 4}, //NTR
                {0, 2, 5}, //FTR
                {0, 3, 4}, //NBR
                {0, 3, 5}, //FBR
                {1, 2, 4}, //NTL
                {1, 2, 5}, //FTL
                {1, 3, 4}, //NBL
                {1, 3, 5} //FBL
            }};

            PlaneSurfaceSplitService planeSurfaceSplitService;
            TerrainSplitService terrainSplitService;
    };

}
