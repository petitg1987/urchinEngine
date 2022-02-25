#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <set>
#include <UrchinCommon.h>

#include <input/AIWorld.h>
#include <path/navmesh/model/NavObject.h>
#include <path/navmesh/model/output/NavMeshAgent.h>
#include <path/navmesh/model/output/NavMesh.h>
#include <path/navmesh/model/output/NavPolygon.h>
#include <path/navmesh/polytope/Polytope.h>
#include <path/navmesh/polytope/PolytopeSurface.h>
#include <path/navmesh/csg/CSGPolygon.h>
#include <path/navmesh/triangulation/TriangulationAlgorithm.h>

namespace urchin {

    class NavMeshGenerator {
        public:
            NavMeshGenerator();

            void setNavMeshAgent(std::unique_ptr<NavMeshAgent>);
            const NavMeshAgent* getNavMeshAgent() const;

            std::shared_ptr<NavMesh> generate(AIWorld&);
            NavMesh copyLastGeneratedNavMesh() const;

        private:
            void updateExpandedPolytopes(AIWorld&);
            void addNavObject(AIEntity&, std::unique_ptr<Polytope>&&);
            void removeNavObject(const AIEntity&);

            void prepareNavObjectsToUpdate();
            void updateNearObjects(NavObject&);

            void updateNavPolygons();
            std::vector<std::shared_ptr<NavPolygon>> createNavigationPolygons(NavObject&, const PolytopeSurface&);
            std::vector<CSGPolygon<float>>& determineObstacles(NavObject&, const PolytopeSurface&) const;
            CSGPolygon<float> computePolytopeFootprint(const Polytope&, const PolytopeSurface&) const;
            void applyObstaclesOnWalkablePolygon(std::vector<CSGPolygon<float>>&);
            std::shared_ptr<NavPolygon> createNavigationPolygon(const CSGPolygon<float>&, const PolytopeSurface&, bool) const;
            std::vector<Point3<float>> elevateTriangulatedPoints(const std::vector<std::vector<Point2<float>>>&, const PolytopeSurface&) const;

            void deleteNavLinks() const;
            void createNavLinks() const;
            void createNavLinks(const NavPolygonEdge&, const NavObject&) const;

            void updateNavMesh();

            const float polygonMinDotProductThreshold;
            const float polygonMergePointsDistanceThreshold;

            mutable std::mutex navMeshMutex;
            std::unique_ptr<NavMeshAgent> navMeshAgent;
            std::shared_ptr<NavMesh> navMesh;
            std::atomic_bool needFullRefresh;

            AABBTree<std::shared_ptr<NavObject>> navigationObjects;
            std::set<std::shared_ptr<NavObject>> newOrMovingNavObjectsToRefresh;
            std::set<std::shared_ptr<NavObject>> affectedNavObjectsToRefresh;
            std::set<std::shared_ptr<NavObject>> navObjectsToRefresh;
            std::set<std::pair<std::shared_ptr<NavObject>, std::shared_ptr<NavObject>>> navObjectsLinksToRefresh;
            std::vector<CSGPolygon<float>> walkablePolygons;
            mutable std::vector<std::shared_ptr<NavObject>> nearObjects;

            std::vector<CSGPolygon<float>> obstaclesInsideWalkablePolygon;
            mutable std::vector<CSGPolygon<float>> holePolygons;
            mutable std::vector<Point2<float>> footprintPoints;
            mutable std::string navPolygonName;

            std::vector<std::shared_ptr<NavObject>> allNavObjects;
            std::vector<std::shared_ptr<NavPolygon>> allNavPolygons;
    };

}
