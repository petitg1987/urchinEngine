#include <cmath>
#include <chrono>
#include <algorithm>
#include <string>
#include <numeric>
#include <path/navmesh/polytope/PolytopeTerrainSurface.h>

#include "NavMeshGenerator.h"
#include "input/AIObject.h"
#include "input/AITerrain.h"
#include "path/navmesh/polytope/PolytopePlaneSurface.h"
#include "path/navmesh/polytope/PolytopeBuilder.h"
#include "path/navmesh/csg/PolygonsUnion.h"
#include "path/navmesh/csg/PolygonsSubtraction.h"

#define WALKABLE_FACE_EXPAND_SIZE 0.0001f
#define OBSTACLE_REDUCE_SIZE 0.0001f

namespace urchin
{

	PolytopeSurfaceIndex::PolytopeSurfaceIndex(it_polytope polytopeRef, unsigned int faceIndex) :
			polytopeRef(polytopeRef),
            faceIndex(faceIndex)
	{

	}

	bool AIEntityComp::operator() (const std::shared_ptr<AIEntity>& left, const std::shared_ptr<AIEntity>& right) const
	{
		return left->getName().compare(right->getName());
	}

    NavMeshGenerator::NavMeshGenerator() :
            polygonMinDotProductThreshold(std::cos(AngleConverter<float>::toRadian(ConfigService::instance()->getFloatValue("navMesh.polygon.removeAngleThresholdInDegree")))),
            polygonMergePointsDistanceThreshold(ConfigService::instance()->getFloatValue("navMesh.polygon.mergePointsDistanceThreshold")),
			navMeshConfig(std::make_shared<NavMeshConfig>(NavMeshAgent())),
			navMesh(std::make_shared<NavMesh>())
    {
		needFullRefresh.store(false, std::memory_order_relaxed);
	}

	void NavMeshGenerator::setNavMeshConfig(std::shared_ptr<NavMeshConfig> navMeshConfig)
	{
		std::lock_guard<std::mutex> lock(navMeshMutex);

		this->navMeshConfig = std::move(navMeshConfig);
        this->needFullRefresh.store(true, std::memory_order_relaxed);
	}

    NavMesh NavMeshGenerator::retrieveLastGeneratedNavMesh() const
    {
        std::lock_guard<std::mutex> lock(navMeshMutex);

        return NavMesh(*navMesh);
    }

	std::shared_ptr<NavMesh> NavMeshGenerator::generate(AIWorld &aiWorld)
	{
		#ifdef _DEBUG
//			auto frameStartTime = std::chrono::high_resolution_clock::now();
        #endif

		updateExpandedPolytopes(aiWorld);
		std::vector<PolytopeSurfaceIndex> polytopeWalkableSurfaces = findWalkableSurfaces();

		std::lock_guard<std::mutex> lock(navMeshMutex);
        navMesh.reset(new NavMesh());
        for (const auto &polytopeWalkableSurface : polytopeWalkableSurfaces)
        {
            std::vector<std::shared_ptr<NavPolygon>> navPolygons = createNavigationPolygonFor(polytopeWalkableSurface);
            for (const auto &navPolygon : navPolygons)
            {
                navMesh->addPolygon(navPolygon);
            }
        }

		#ifdef _DEBUG
//			auto frameEndTime = std::chrono::high_resolution_clock::now();
//			auto diffTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
//			std::cout<<"Nav mesh generation time (ms): "<<diffTimeMicroSeconds/1000.0<<std::endl;
		#endif

		return navMesh;
	}

	void NavMeshGenerator::updateExpandedPolytopes(AIWorld &aiWorld)
	{
		for(auto &aiObjectToRemove : aiWorld.getEntitiesToRemoveAndReset())
		{
			expandedPolytopes.erase(aiObjectToRemove);
		}

        bool refreshAllEntities = needFullRefresh.exchange(false, std::memory_order_relaxed);
		for(auto &aiEntity : aiWorld.getEntities())
		{
			if(aiEntity->isToRebuild() || refreshAllEntities)
			{
				expandedPolytopes.erase(aiEntity);

                if(aiEntity->getType()==AIEntity::OBJECT)
                {
                    auto aiObject = std::dynamic_pointer_cast<AIObject>(aiEntity);
                    std::vector<std::unique_ptr<Polytope>> objectExpandedPolytopes = PolytopeBuilder::instance()->buildExpandedPolytopes(aiObject, navMeshConfig->getAgent());
                    for (auto &objectExpandedPolytope : objectExpandedPolytopes)
                    {
                        expandedPolytopes.insert(std::pair<std::shared_ptr<AIEntity>, std::unique_ptr<Polytope>>(aiObject, std::move(objectExpandedPolytope)));
                    }
                }else if(aiEntity->getType()==AIEntity::TERRAIN)
                {
					auto aiTerrain = std::dynamic_pointer_cast<AITerrain>(aiEntity);
					std::unique_ptr<Polytope> terrainExpandedPolytope = PolytopeBuilder::instance()->buildExpandedPolytope(aiTerrain, navMeshConfig);

					expandedPolytopes.insert(std::pair<std::shared_ptr<AIEntity>, std::unique_ptr<Polytope>>(aiTerrain, std::move(terrainExpandedPolytope)));
                }

                aiEntity->markRebuilt();
			}
		}
	}

	std::vector<PolytopeSurfaceIndex> NavMeshGenerator::findWalkableSurfaces() const
	{
		std::vector<PolytopeSurfaceIndex> walkableFaces;
		walkableFaces.reserve(expandedPolytopes.size()/8); //estimated memory size

        for(auto itPolytope = expandedPolytopes.begin(); itPolytope!=expandedPolytopes.end(); ++itPolytope)
		{
			const std::unique_ptr<Polytope> &expandedPolytope = itPolytope->second;
			if(expandedPolytope->isWalkableCandidate())
			{
				for(unsigned int surfaceIndex=0; surfaceIndex<expandedPolytope->getSurfaces().size(); ++surfaceIndex)
				{
					const std::unique_ptr<PolytopeSurface> &surface = expandedPolytope->getSurface(surfaceIndex);

					if(surface->isWalkable(navMeshConfig->getMaxSlope()))
					{
						walkableFaces.emplace_back(PolytopeSurfaceIndex(itPolytope, surfaceIndex));
					}
				}
			}
		}

		return walkableFaces;
	}

	std::vector<std::shared_ptr<NavPolygon>> NavMeshGenerator::createNavigationPolygonFor(const PolytopeSurfaceIndex &polytopeWalkableSurface) const
	{
		const std::unique_ptr<Polytope> &polytope = polytopeWalkableSurface.polytopeRef->second;
		const std::unique_ptr<PolytopeSurface> &walkableFace = polytope->getSurface(polytopeWalkableSurface.faceIndex);

		std::string walkableName = polytope->getName() + "[" + std::to_string(polytopeWalkableSurface.faceIndex) + "]";
        std::vector<CSGPolygon<float>> walkablePolygons = {CSGPolygon<float>(walkableName, walkableFace->getOutlineCwPoints())};

        auto obstaclePolygons = computeObstacles(polytopeWalkableSurface);
		std::vector<CSGPolygon<float>> remainingObstaclePolygons;
		remainingObstaclePolygons.reserve(2); //estimated memory size

		for(const auto &obstaclePolygon : obstaclePolygons)
		{
            CSGPolygon<float> simplifiedObstaclePolygon = obstaclePolygon.simplify(polygonMinDotProductThreshold, polygonMergePointsDistanceThreshold);
            if (simplifiedObstaclePolygon.getCwPoints().size() > 2)
            {
                auto walkablePolygonsCounter = static_cast<int>(walkablePolygons.size());
                while (walkablePolygonsCounter-- != 0)
                {
                    const CSGPolygon<float> &walkablePolygon = walkablePolygons[0];

                    bool obstacleInsideWalkable;
                    std::vector<CSGPolygon<float>> subtractedPolygons = PolygonsSubtraction<float>::instance()->subtractPolygons(
                            walkablePolygon, simplifiedObstaclePolygon, obstacleInsideWalkable);

                    //replace 'walkablePolygon' by 'subtractedPolygons'
                    walkablePolygons.erase(walkablePolygons.begin());
                    for (auto &subtractedPolygon : subtractedPolygons)
                    {
                        walkablePolygons.emplace_back(subtractedPolygon);
                    }

                    if (obstacleInsideWalkable)
                    {
                        //slightly reduce to avoid obstacle points touch others obstacles points (not supported by triangulation)
                        std::vector<Point2<float>> reducedObstacleCwPoints = ResizePolygon2DService<float>::instance()->resizePolygon(
                                simplifiedObstaclePolygon.getCwPoints(), OBSTACLE_REDUCE_SIZE);

                        remainingObstaclePolygons.emplace_back(CSGPolygon<float>(simplifiedObstaclePolygon.getName(), reducedObstacleCwPoints));
                    }
                }
            }
		}

		std::vector<std::shared_ptr<NavPolygon>> navPolygons;
		navPolygons.reserve(walkablePolygons.size());
		for(auto &walkablePolygon : walkablePolygons)
		{
            //simplify polygon to improve performance and avoid degenerated walkable face
			CSGPolygon<float> simplifiedWalkablePolygons = walkablePolygon.simplify(polygonMinDotProductThreshold, polygonMergePointsDistanceThreshold);
            if(simplifiedWalkablePolygons.getCwPoints().size() > 2)
            {
                //slightly expand to avoid obstacle points to be in contact with walkable edges (not supported by triangulation)
                std::vector<Point2<float>> extendedWalkableCwPoints = ResizePolygon2DService<float>::instance()->resizePolygon(
                        simplifiedWalkablePolygons.getCwPoints(), -WALKABLE_FACE_EXPAND_SIZE);

				std::string navPolygonName = "<" + simplifiedWalkablePolygons.getName() + ">";
				TriangulationAlgorithm triangulation(reversePoints(extendedWalkableCwPoints));

                for(const auto &remainingObstaclePolygon : remainingObstaclePolygons)
                {
                    if(simplifiedWalkablePolygons.pointInsideOrOnPolygon(remainingObstaclePolygon.getCwPoints()[0]))
                    { //obstacle fully inside walkable polygon
                        triangulation.addHolePoints(remainingObstaclePolygon.getCwPoints());
						navPolygonName += " - <" + remainingObstaclePolygon.getName() + ">";
                    }
                }

                std::vector<Point3<float>> points = elevateTriangulatedPoints(triangulation, walkableFace);
                navPolygons.push_back(std::make_shared<NavPolygon>(navPolygonName, points, triangulation.triangulate()));
            }
		}

		return navPolygons;
	}

	std::vector<Point2<float>> NavMeshGenerator::reversePoints(const std::vector<Point2<float>> &points) const
	{
		std::vector<Point2<float>> reversePoints;
		reversePoints.reserve(points.size());

		for(auto it = points.rbegin(); it!=points.rend(); ++it)
		{
			reversePoints.emplace_back(*it);
		}

		return reversePoints;
	}

	std::vector<CSGPolygon<float>> NavMeshGenerator::computeObstacles(const PolytopeSurfaceIndex &polytopeWalkableSurface) const
	{
		const std::unique_ptr<Polytope> &polytope = polytopeWalkableSurface.polytopeRef->second;
		const std::unique_ptr<PolytopeSurface> &walkableSurface = polytope->getSurface(polytopeWalkableSurface.faceIndex);

		std::vector<CSGPolygon<float>> holePolygons = walkableSurface->getSelfObstacles();
        for (const auto &expandedPolytopeObstacle : expandedPolytopes)
        {
			if(expandedPolytopeObstacle.first!=polytopeWalkableSurface.polytopeRef->first && expandedPolytopeObstacle.second->isObstacleCandidate())
			{
				CSGPolygon<float> footprintPolygon = computePolytopeFootprint(expandedPolytopeObstacle.second, walkableSurface);
				if(footprintPolygon.getCwPoints().size() >= 3)
				{
					holePolygons.push_back(footprintPolygon);
				}
			}
		}

		return PolygonsUnion<float>::instance()->unionPolygons(holePolygons);
	}

	CSGPolygon<float> NavMeshGenerator::computePolytopeFootprint(const std::unique_ptr<Polytope> &polytopeObstacle, const std::unique_ptr<PolytopeSurface> &walkableSurface) const
	{
		std::vector<Point2<float>> footprintPoints;
        footprintPoints.reserve(4); //estimated memory size

        Plane<float> walkablePlane = walkableSurface->getPlane(*polytopeObstacle->getXZRectangle(), navMeshConfig->getAgent());

		for(const auto &polytopeSurface : polytopeObstacle->getSurfaces())
		{
            if(auto *polytopePlaneSurface = dynamic_cast<PolytopePlaneSurface *>(polytopeSurface.get()))
            {
                for (unsigned int i = 0, previousI = polytopePlaneSurface->getCcwPoints().size() - 1; i < polytopePlaneSurface->getCcwPoints().size(); previousI = i++)
                {
                    float distance1 = walkablePlane.distance(polytopePlaneSurface->getCcwPoints()[previousI]);
                    float distance2 = walkablePlane.distance(polytopePlaneSurface->getCcwPoints()[i]);
                    if ((distance1 < 0.0 && distance2 > 0.0) || (distance1 > 0.0 && distance2 < 0.0))
                    {
                        Line3D<float> polytopeEdgeLine(polytopePlaneSurface->getCcwPoints()[previousI], polytopePlaneSurface->getCcwPoints()[i]);
                        Point3<float> intersectionPoint = walkablePlane.intersectPoint(polytopeEdgeLine);
                        footprintPoints.emplace_back(Point2<float>(intersectionPoint.X, -intersectionPoint.Z));
                    }
                }
            }else
            {
                throw std::runtime_error("Unsupported type of surface as obstacle: " + std::string(typeid(*polytopeSurface).name()));
            }
		}

		ConvexHull2D<float> footprintConvexHull(footprintPoints);
		std::vector<Point2<float>> cwPoints(footprintConvexHull.getPoints());
		std::reverse(cwPoints.begin(), cwPoints.end());
		return CSGPolygon<float>(polytopeObstacle->getName(), cwPoints);
	}

	std::vector<Point3<float>> NavMeshGenerator::elevateTriangulatedPoints(const TriangulationAlgorithm &triangulation, const std::unique_ptr<PolytopeSurface> &walkableSurface) const
	{
		std::vector<Point3<float>> elevatedPoints;
		elevatedPoints.reserve(triangulation.getAllPointsSize());

        for(const auto &walkablePoint : triangulation.getPolygonPoints())
        {
            elevatedPoints.push_back(walkableSurface->computeRealPoint(walkablePoint, navMeshConfig->getAgent()));
        }

		for(unsigned int holeIndex=0; holeIndex<triangulation.getHolesSize(); ++holeIndex)
		{
			const std::vector<Point2<float>> &holePoints = triangulation.getHolePoints(holeIndex);
			for(const auto &holePoint : holePoints)
			{
				elevatedPoints.push_back(walkableSurface->computeRealPoint(holePoint, navMeshConfig->getAgent()));
			}
		}

		return elevatedPoints;
	}

}
