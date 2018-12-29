#ifndef URCHINENGINE_NAVMESHGENERATOR_H
#define URCHINENGINE_NAVMESHGENERATOR_H

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <map>
#include "UrchinCommon.h"

#include "input/AIWorld.h"
#include "path/navmesh/model/NavMeshConfig.h"
#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavPolygon.h"
#include "path/navmesh/polytope/Polytope.h"
#include "path/navmesh/polytope/PolytopeSurface.h"
#include "path/navmesh/csg/CSGPolygon.h"
#include "path/navmesh/triangulation/TriangulationAlgorithm.h"

namespace urchin
{

	typedef std::map<std::shared_ptr<AIEntity>, std::unique_ptr<Polytope>>::const_iterator it_polytope;

	struct PolytopeSurfaceIndex
	{
		PolytopeSurfaceIndex(it_polytope, unsigned int);

		it_polytope polytopeRef;
		unsigned int faceIndex;
	};

	struct AIEntityComp
	{
		bool operator() (const std::shared_ptr<AIEntity>& left, const std::shared_ptr<AIEntity>& right) const;
	};

	class NavMeshGenerator
	{
		public:
            NavMeshGenerator();

			void setNavMeshConfig(std::shared_ptr<NavMeshConfig>);

			std::shared_ptr<NavMesh> generate(AIWorld &);
			NavMesh retrieveLastGeneratedNavMesh() const;

		private:
			void updateExpandedPolytopes(AIWorld &);

			std::vector<PolytopeSurfaceIndex> findWalkableSurfaces() const;

			std::vector<std::shared_ptr<NavPolygon>> createNavigationPolygon(const PolytopeSurfaceIndex &) const;
			std::vector<CSGPolygon<float>> &computeObstacles(const PolytopeSurfaceIndex &) const;
			CSGPolygon<float> computePolytopeFootprint(const std::unique_ptr<Polytope> &, const std::unique_ptr<PolytopeSurface> &) const;
			std::vector<Point3<float>> elevateTriangulatedPoints(const TriangulationAlgorithm &, const std::unique_ptr<PolytopeSurface> &) const;

			const float polygonMinDotProductThreshold;
			const float polygonMergePointsDistanceThreshold;

            mutable std::mutex navMeshMutex;
			std::shared_ptr<NavMeshConfig> navMeshConfig;
			std::multimap<std::shared_ptr<AIEntity>, std::unique_ptr<Polytope>, AIEntityComp> expandedPolytopes;

			std::shared_ptr<NavMesh> navMesh;
			std::atomic_bool needFullRefresh;

			mutable std::vector<CSGPolygon<float>> holePolygons;
			mutable std::vector<Point2<float>> footprintPoints;
			mutable std::vector<CSGPolygon<float>> remainingObstaclePolygons;
			mutable std::vector<CSGPolygon<float>> walkablePolygons;
	};

}

#endif
