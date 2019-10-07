#include <cmath>
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

    NavMeshGenerator::NavMeshGenerator() :
            polygonMinDotProductThreshold(std::cos(AngleConverter<float>::toRadian(ConfigService::instance()->getFloatValue("navMesh.polygon.removeAngleThresholdInDegree")))),
            polygonMergePointsDistanceThreshold(ConfigService::instance()->getFloatValue("navMesh.polygon.mergePointsDistanceThreshold")),
			navMeshConfig(std::make_shared<NavMeshConfig>(NavMeshAgent())),
			navMesh(std::make_shared<NavMesh>()),
			needFullRefresh(false)
    {

	}

	void NavMeshGenerator::setNavMeshConfig(std::shared_ptr<NavMeshConfig> navMeshConfig)
	{
		std::lock_guard<std::mutex> lock(navMeshMutex);

		this->navMeshConfig = std::move(navMeshConfig);
        this->needFullRefresh.store(true, std::memory_order_relaxed);
	}

    NavMesh NavMeshGenerator::copyLastGeneratedNavMesh() const
    {
        std::lock_guard<std::mutex> lock(navMeshMutex);

        return NavMesh(*navMesh);
    }

	std::shared_ptr<NavMesh> NavMeshGenerator::generate(AIWorld &aiWorld)
	{
		ScopeProfiler scopeProfiler("ai", "navMeshGenerate");

		updateExpandedPolytopes(aiWorld);

        allNavPolygons.clear();
        for (const auto &polytopeWalkableSurface : walkableSurfaces)
        {
            std::vector<std::shared_ptr<NavPolygon>> navPolygons = createNavigationPolygons(polytopeWalkableSurface.second);
            allNavPolygons.insert(allNavPolygons.end(), navPolygons.begin(), navPolygons.end());
        }

        std::lock_guard<std::mutex> lock(navMeshMutex);
        navMesh->replaceAllPolygons(allNavPolygons);
		return navMesh;
	}

	void NavMeshGenerator::updateExpandedPolytopes(AIWorld &aiWorld)
	{
        ScopeProfiler scopeProfiler("ai", "upExpandPoly");

		for(auto &aiObjectToRemove : aiWorld.getEntitiesToRemoveAndReset())
		{
            removeExpandedPolygon(aiObjectToRemove);
		}

        bool refreshAllEntities = needFullRefresh.exchange(false, std::memory_order_relaxed);
		for(auto &aiEntity : aiWorld.getEntities())
		{
			if(aiEntity->isToRebuild() || refreshAllEntities)
			{
                removeExpandedPolygon(aiEntity);

                if(aiEntity->getType()==AIEntity::OBJECT)
                {
                    auto aiObject = std::dynamic_pointer_cast<AIObject>(aiEntity);
                    std::vector<std::unique_ptr<Polytope>> objectExpandedPolytopes = PolytopeBuilder::instance()->buildExpandedPolytopes(aiObject, navMeshConfig);
                    for (auto &objectExpandedPolytope : objectExpandedPolytopes)
                    {
                        addExpandedPolygon(aiObject, std::move(objectExpandedPolytope));
                    }
                }else if(aiEntity->getType()==AIEntity::TERRAIN)
                {
					auto aiTerrain = std::dynamic_pointer_cast<AITerrain>(aiEntity);
					std::unique_ptr<Polytope> terrainExpandedPolytope = PolytopeBuilder::instance()->buildExpandedPolytope(aiTerrain, navMeshConfig);
                    addExpandedPolygon(aiTerrain, std::move(terrainExpandedPolytope));
                }

                aiEntity->markRebuilt();
			}
		}
	}

	void NavMeshGenerator::addExpandedPolygon(const std::shared_ptr<AIEntity> &aiEntity, const std::shared_ptr<Polytope>& expandedPolytope)
    {
        if(expandedPolytope->isWalkableCandidate())
        {
            for(std::size_t surfaceIndex=0; surfaceIndex<expandedPolytope->getSurfaces().size(); ++surfaceIndex)
            {
                const std::shared_ptr<PolytopeSurface> &polytopeSurface = expandedPolytope->getSurface(surfaceIndex);
                if(polytopeSurface->isWalkable())
                {
                    walkableSurfaces.insert(std::pair<std::shared_ptr<Polytope>, std::shared_ptr<PolytopeSurface>>(expandedPolytope, polytopeSurface));
                }
            }
        }

        expandedPolytopes.push_back(expandedPolytope);
        aiEntity->addExpandedPolytope(expandedPolytope);
    }

    void NavMeshGenerator::removeExpandedPolygon(const std::shared_ptr<AIEntity> &aiEntity)
    {
        for(const auto &expandedPolytope : aiEntity->getExpandedPolytopes())
        {
            expandedPolytopes.erase(std::remove(expandedPolytopes.begin(), expandedPolytopes.end(), expandedPolytope), expandedPolytopes.end());
            walkableSurfaces.erase(expandedPolytope);
        }
    }

	std::vector<std::shared_ptr<NavPolygon>> NavMeshGenerator::createNavigationPolygons(const std::shared_ptr<PolytopeSurface> &walkableSurface) const
	{
		ScopeProfiler scopeProfiler("ai", "createNavPolys");

        walkablePolygons.clear();
        remainingObstaclePolygons.clear();

        std::string walkableName = walkableSurface->getPolytope()->getName() + "[" + std::to_string(walkableSurface->getSurfacePosition()) + "]";
        walkablePolygons.emplace_back(CSGPolygon<float>(walkableName, walkableSurface->getOutlineCwPoints()));

        std::vector<CSGPolygon<float>> &obstaclePolygons = determineObstacles(walkableSurface);
        subtractObstaclesOnOutline(obstaclePolygons);

		std::vector<std::shared_ptr<NavPolygon>> navPolygons;
		navPolygons.reserve(walkablePolygons.size());
		for(auto &walkablePolygon : walkablePolygons)
		{
            //simplify polygon to improve performance and avoid degenerated walkable face
			walkablePolygon.simplify(polygonMinDotProductThreshold, polygonMergePointsDistanceThreshold);
            if(walkablePolygon.getCwPoints().size() > 2)
            {
                std::shared_ptr<NavPolygon> navPolygon = createNavigationPolygon(walkablePolygon, walkableSurface);
                navPolygons.push_back(navPolygon);
            }
		}

		return navPolygons;
	}

	std::vector<CSGPolygon<float>> &NavMeshGenerator::determineObstacles(const std::shared_ptr<PolytopeSurface> &walkableSurface) const
	{
		ScopeProfiler scopeProfiler("ai", "getObstacles");

		const std::vector<CSGPolygon<float>> &selfObstaclePolygons = walkableSurface->getSelfObstacles();

        holePolygons.clear();
        for(const auto &selfObstaclePolygon : selfObstaclePolygons)
        {
            holePolygons.emplace_back(selfObstaclePolygon);
        }
        for (const auto &expandedPolytopeObstacle : expandedPolytopes)
        {
			if(expandedPolytopeObstacle->getName()!=walkableSurface->getPolytope()->getName() && expandedPolytopeObstacle->isObstacleCandidate())
			{
				CSGPolygon<float> footprintPolygon = computePolytopeFootprint(expandedPolytopeObstacle, walkableSurface);
				if(footprintPolygon.getCwPoints().size() >= 3)
				{
                    footprintPolygon.simplify(polygonMinDotProductThreshold, polygonMergePointsDistanceThreshold);
					holePolygons.push_back(std::move(footprintPolygon));
				}
			}
		}

		return PolygonsUnion<float>::instance()->unionPolygons(holePolygons);
	}

	CSGPolygon<float> NavMeshGenerator::computePolytopeFootprint(const std::shared_ptr<Polytope> &polytopeObstacle, const std::shared_ptr<PolytopeSurface> &walkableSurface) const
	{
		footprintPoints.clear();
        Plane<float> walkablePlane = walkableSurface->getPlane(polytopeObstacle->getXZRectangle(), navMeshConfig->getAgent());

		for(const auto &polytopeSurface : polytopeObstacle->getSurfaces())
		{
            if(auto *polytopePlaneSurface = dynamic_cast<PolytopePlaneSurface *>(polytopeSurface.get()))
            {
                for (std::size_t i = 0, previousI = polytopePlaneSurface->getCcwPoints().size() - 1; i < polytopePlaneSurface->getCcwPoints().size(); previousI = i++)
                {
                    float distance1 = walkablePlane.distance(polytopePlaneSurface->getCcwPoints()[previousI]);
                    float distance2 = walkablePlane.distance(polytopePlaneSurface->getCcwPoints()[i]);
                    if (std::signbit(distance1) != std::signbit(distance2))
                    {
                        Line3D<float> polytopeEdgeLine(polytopePlaneSurface->getCcwPoints()[previousI], polytopePlaneSurface->getCcwPoints()[i]);
                        Point3<float> intersectionPoint = walkablePlane.intersectPoint(polytopeEdgeLine);
                        footprintPoints.emplace_back(Point2<float>(intersectionPoint.X, -intersectionPoint.Z));
                    }
                }
            }else
            {
                throw std::runtime_error("Unsupported type of surface as obstacle: " + std::string(typeid(polytopeSurface.get()).name()));
            }
		}

		ConvexHull2D<float> footprintConvexHull(footprintPoints);
		std::vector<Point2<float>> cwPoints(footprintConvexHull.getPoints());
		std::reverse(cwPoints.begin(), cwPoints.end());
		return CSGPolygon<float>(polytopeObstacle->getName(), std::move(cwPoints));
	}

	void NavMeshGenerator::subtractObstaclesOnOutline(std::vector<CSGPolygon<float>> &obstaclePolygons) const
    {
        ScopeProfiler scopeProfiler("ai", "subObstacles");

        for(auto &obstaclePolygon : obstaclePolygons)
        {
            obstaclePolygon.simplify(polygonMinDotProductThreshold, polygonMergePointsDistanceThreshold);
            if (obstaclePolygon.getCwPoints().size() > 2)
            {
                auto walkablePolygonsCounter = static_cast<int>(walkablePolygons.size());
                while (walkablePolygonsCounter-- != 0)
                {
                    const CSGPolygon<float> &walkablePolygon = walkablePolygons[0];

                    bool obstacleInsideWalkable;
                    const std::vector<CSGPolygon<float>> &subtractedPolygons = PolygonsSubtraction<float>::instance()->subtractPolygons(
                            walkablePolygon, obstaclePolygon, obstacleInsideWalkable);

                    //replace 'walkablePolygon' by 'subtractedPolygons'
                    walkablePolygons.erase(walkablePolygons.begin());
                    for (auto &subtractedPolygon : subtractedPolygons)
                    {
                        walkablePolygons.emplace_back(subtractedPolygon);
                    }

                    if (obstacleInsideWalkable)
                    {
                        //slightly reduce to avoid obstacle points touch others obstacles points (not supported by triangulation)
                        obstaclePolygon.expand(-OBSTACLE_REDUCE_SIZE);
                        remainingObstaclePolygons.emplace_back(obstaclePolygon);
                    }
                }
            }
        }
    }

    std::shared_ptr<NavPolygon> NavMeshGenerator::createNavigationPolygon(CSGPolygon<float> &walkablePolygon, const std::shared_ptr<PolytopeSurface> &walkableSurface) const
    {
        ScopeProfiler scopeProfiler("ai", "createNavPoly");

        //slightly expand to avoid obstacle points to be in contact with walkable edges (not supported by triangulation)
        walkablePolygon.expand(WALKABLE_FACE_EXPAND_SIZE);
        std::vector<Point2<float>> walkablePolygonPoints = walkablePolygon.getCwPoints();

        navPolygonName = "<" + walkablePolygon.getName() + ">";
        std::reverse(walkablePolygonPoints.begin(), walkablePolygonPoints.end()); //CW to CCW
        TriangulationAlgorithm triangulation(std::move(walkablePolygonPoints), walkablePolygon.getName(), TriangulationAlgorithm::CCW);

        for(const auto &remainingObstaclePolygon : remainingObstaclePolygons)
        {
            if(walkablePolygon.pointInsideOrOnPolygon(remainingObstaclePolygon.getCwPoints()[0]))
            { //obstacle fully inside walkable polygon
                triangulation.addHolePoints(remainingObstaclePolygon.getCwPoints(), remainingObstaclePolygon.getName());
                navPolygonName += " - <" + remainingObstaclePolygon.getName() + ">";
            }
        }

        std::vector<Point3<float>> points = elevateTriangulatedPoints(triangulation, walkableSurface);
        NavTopography *navTopography = walkableSurface->newNavTopography();

        std::shared_ptr<NavPolygon> navPolygon = std::make_shared<NavPolygon>(navPolygonName, std::move(points), navTopography);
        navPolygon->addTriangles(triangulation.triangulate(), navPolygon);

        return std::move(navPolygon);
    }

	std::vector<Point3<float>> NavMeshGenerator::elevateTriangulatedPoints(const TriangulationAlgorithm &triangulation, const std::shared_ptr<PolytopeSurface> &walkableSurface) const
	{
		ScopeProfiler scopeProfiler("ai", "elevateTriPoint");

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
