#ifndef URCHINENGINE_NAVMESHGENERATOR_H
#define URCHINENGINE_NAVMESHGENERATOR_H

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <set>
#include "UrchinCommon.h"

#include "input/AIWorld.h"
#include "path/navmesh/model/NavObject.h"
#include "path/navmesh/model/output/NavMeshAgent.h"
#include "path/navmesh/model/output/NavMesh.h"
#include "path/navmesh/model/output/NavPolygon.h"
#include "path/navmesh/polytope/Polytope.h"
#include "path/navmesh/polytope/PolytopeSurface.h"
#include "path/navmesh/csg/CSGPolygon.h"
#include "path/navmesh/triangulation/TriangulationAlgorithm.h"

namespace urchin {

    class NavMeshGenerator {
        public:
            NavMeshGenerator();

            void setNavMeshAgent(std::shared_ptr<NavMeshAgent>);
            const std::shared_ptr<NavMeshAgent> &getNavMeshAgent() const;

            std::shared_ptr<NavMesh> generate(AIWorld &);
            NavMesh copyLastGeneratedNavMesh() const;

        private:
            void updateExpandedPolytopes(AIWorld &);
            void addNavObject(const std::shared_ptr<AIEntity>&, std::unique_ptr<Polytope> &&);
            void removeNavObject(const std::shared_ptr<AIEntity>&);

            void prepareNavObjectsToUpdate();
            void updateNearObjects(const std::shared_ptr<NavObject>&);

            void updateNavPolygons();
            std::vector<std::shared_ptr<NavPolygon>> createNavigationPolygons(const std::shared_ptr<NavObject>&, const std::shared_ptr<PolytopeSurface>&);
            std::vector<CSGPolygon<float>> &determineObstacles(const std::shared_ptr<NavObject>&, const std::shared_ptr<PolytopeSurface>&) const;
            CSGPolygon<float> computePolytopeFootprint(const std::shared_ptr<Polytope>&, const std::shared_ptr<PolytopeSurface>&) const;
            void applyObstaclesOnWalkablePolygon(std::vector<CSGPolygon<float>> &);
            std::shared_ptr<NavPolygon> createNavigationPolygon(CSGPolygon<float> &, const std::shared_ptr<PolytopeSurface>&, bool) const;
            std::vector<Point3<float>> elevateTriangulatedPoints(const TriangulationAlgorithm&, const std::shared_ptr<PolytopeSurface>&) const;

            void deleteNavLinks();
            void createNavLinks();
            void createNavLinks(const NavPolygonEdge&, const std::shared_ptr<NavObject>&) const;

            void updateNavMesh();

            const float polygonMinDotProductThreshold;
            const float polygonMergePointsDistanceThreshold;

            mutable std::mutex navMeshMutex;
            std::shared_ptr<NavMeshAgent> navMeshAgent;
            std::shared_ptr<NavMesh> navMesh;
            std::atomic_bool needFullRefresh;

            AABBTree<std::shared_ptr<NavObject>> navigationObjects;
            std::set<std::shared_ptr<NavObject>> newOrMovingNavObjectsToRefresh, affectedNavObjectsToRefresh;
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

#endif
