#ifndef URCHINENGINE_NAVMESHGENERATOR_H
#define URCHINENGINE_NAVMESHGENERATOR_H

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <map>
#include "UrchinCommon.h"

#include "input/AIWorld.h"
#include "path/navmesh/model/NavMeshAgent.h"
#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavPolygon.h"
#include "path/navmesh/polytope/Polytope.h"
#include "path/navmesh/polytope/PolytopeSurface.h"
#include "path/navmesh/csg/CSGPolygon.h"
#include "path/navmesh/triangulation/TriangulationAlgorithm.h"

namespace urchin
{

	class NavMeshGenerator
	{
		public:
            NavMeshGenerator();

			void setNavMeshAgent(std::shared_ptr<NavMeshAgent>);

			std::shared_ptr<NavMesh> generate(AIWorld &);
			NavMesh copyLastGeneratedNavMesh() const;

		private:
			void updateExpandedPolytopes(AIWorld &);
            void addExpandedPolygon(const std::shared_ptr<AIEntity> &, const std::shared_ptr<Polytope> &);
            void removeExpandedPolygon(const std::shared_ptr<AIEntity> &);

			std::vector<std::shared_ptr<NavPolygon>> createNavigationPolygons(const std::shared_ptr<PolytopeSurface> &) const;

			std::vector<CSGPolygon<float>> &determineObstacles(const std::shared_ptr<PolytopeSurface> &) const;
			CSGPolygon<float> computePolytopeFootprint(const std::shared_ptr<Polytope> &, const std::shared_ptr<PolytopeSurface> &) const;

            void subtractObstaclesOnOutline(std::vector<CSGPolygon<float>> &) const;

            std::shared_ptr<NavPolygon> createNavigationPolygon(CSGPolygon<float> &, const std::shared_ptr<PolytopeSurface> &) const;
			std::vector<Point3<float>> elevateTriangulatedPoints(const TriangulationAlgorithm &, const std::shared_ptr<PolytopeSurface> &) const;

			const float polygonMinDotProductThreshold;
			const float polygonMergePointsDistanceThreshold;

            mutable std::mutex navMeshMutex;
			std::shared_ptr<NavMeshAgent> navMeshAgent;
            std::shared_ptr<NavMesh> navMesh;
            std::atomic_bool needFullRefresh;

            //TODO AABBTree<std::shared_ptr<Polytope>> expandedPolytopes;
            std::vector<std::shared_ptr<Polytope>> expandedPolytopes;
            std::map<std::shared_ptr<Polytope>, std::shared_ptr<PolytopeSurface>> walkableSurfaces;
            mutable std::vector<CSGPolygon<float>> walkablePolygons;
            mutable std::vector<CSGPolygon<float>> remainingObstaclePolygons;
			mutable std::vector<CSGPolygon<float>> holePolygons;
			mutable std::vector<Point2<float>> footprintPoints;
			mutable std::string navPolygonName;
            std::vector<std::shared_ptr<NavPolygon>> allNavPolygons;
	};

}

#endif
