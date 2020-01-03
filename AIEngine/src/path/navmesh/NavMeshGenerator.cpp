#include <cmath>
#include <algorithm>
#include <string>
#include <numeric>

#include "NavMeshGenerator.h"
#include "input/AIObject.h"
#include "input/AITerrain.h"
#include "path/navmesh/polytope/PolytopePlaneSurface.h"
#include "path/navmesh/polytope/PolytopeBuilder.h"
#include "path/navmesh/polytope/aabbtree/NavObjectAABBNodeData.h"
#include "path/navmesh/csg/PolygonsUnion.h"
#include "path/navmesh/csg/PolygonsSubtraction.h"
#include "path/navmesh/jump/EdgeJumpDetection.h"

#define WALKABLE_FACE_EXPAND_SIZE 0.0001f
#define OBSTACLE_REDUCE_SIZE 0.0001f

namespace urchin
{

    NavMeshGenerator::NavMeshGenerator() :
            polygonMinDotProductThreshold(std::cos(AngleConverter<float>::toRadian(ConfigService::instance()->getFloatValue("navMesh.polygonRemoveAngleThresholdInDegree")))),
            polygonMergePointsDistanceThreshold(ConfigService::instance()->getFloatValue("navMesh.polygonMergePointsDistanceThreshold")),
			navMeshAgent(std::make_shared<NavMeshAgent>()),
			navMesh(std::make_shared<NavMesh>()),
			needFullRefresh(false),
            navigationObjects(AABBTree<std::shared_ptr<NavObject>>(ConfigService::instance()->getFloatValue("navMesh.polytopeAabbTreeFatMargin")))
    {

	}

	void NavMeshGenerator::setNavMeshAgent(std::shared_ptr<NavMeshAgent> navMeshAgent)
	{
		std::lock_guard<std::mutex> lock(navMeshMutex);

		this->navMeshAgent = std::move(navMeshAgent);
        this->needFullRefresh.store(true, std::memory_order_relaxed);

        float navigationObjectsJumpMargin = this->navMeshAgent->getJumpDistance() / 2.0f;
        float navigationObjectsMargin = std::max(navigationObjectsJumpMargin, ConfigService::instance()->getFloatValue("navMesh.polytopeAabbTreeFatMargin"));
        this->navigationObjects.updateFatMargin(navigationObjectsMargin);
	}

    const std::shared_ptr<NavMeshAgent> &NavMeshGenerator::getNavMeshAgent() const
    {
        return navMeshAgent;
    }

    NavMesh NavMeshGenerator::copyLastGeneratedNavMesh() const
    {
        std::lock_guard<std::mutex> lock(navMeshMutex);

        return NavMesh(*navMesh);
    }

    /**
     * See '_doc' for an algorithm overview
     */
	std::shared_ptr<NavMesh> NavMeshGenerator::generate(AIWorld &aiWorld)
	{
		ScopeProfiler scopeProfiler("ai", "navMeshGenerate");

		updateExpandedPolytopes(aiWorld);
        prepareNavObjectsToUpdate();
        deleteNavLinks();
		updateNavPolygons();
		createNavLinks();
        updateNavMesh();

		return navMesh;
	}

	void NavMeshGenerator::updateExpandedPolytopes(AIWorld &aiWorld)
	{
        ScopeProfiler scopeProfiler("ai", "upExpandPoly");

        newOrMovingNavObjectsToRefresh.clear();
        affectedNavObjectsToRefresh.clear();

		for(auto &aiObjectToRemove : aiWorld.getEntitiesToRemoveAndReset())
		{
            removeNavObject(aiObjectToRemove);
		}

        bool refreshAllEntities = needFullRefresh.exchange(false, std::memory_order_relaxed);
		for(auto &aiEntity : aiWorld.getEntities())
		{
			if(aiEntity->isToRebuild() || refreshAllEntities)
			{
                removeNavObject(aiEntity);
                aiEntity->removeAllNavObjects();

                if(aiEntity->getType()==AIEntity::OBJECT)
                {
                    auto aiObject = std::dynamic_pointer_cast<AIObject>(aiEntity);
                    std::vector<std::unique_ptr<Polytope>> objectExpandedPolytopes = PolytopeBuilder::instance()->buildExpandedPolytopes(aiObject, navMeshAgent);
                    for (auto &objectExpandedPolytope : objectExpandedPolytopes)
                    {
                        addNavObject(aiObject, std::move(objectExpandedPolytope));
                    }
                }else if(aiEntity->getType()==AIEntity::TERRAIN)
                {
					auto aiTerrain = std::dynamic_pointer_cast<AITerrain>(aiEntity);
                    std::vector<std::unique_ptr<Polytope>> terrainExpandedPolytopes = PolytopeBuilder::instance()->buildExpandedPolytope(aiTerrain, navMeshAgent);
                    for (auto &terrainExpandedPolytope : terrainExpandedPolytopes)
                    {
                        addNavObject(aiTerrain, std::move(terrainExpandedPolytope));
                    }
                }

                aiEntity->markRebuilt();
			}
		}
	}

	void NavMeshGenerator::addNavObject(const std::shared_ptr<AIEntity> &aiEntity, const std::shared_ptr<Polytope>& expandedPolytope)
    {
        auto navObject = std::make_shared<NavObject>(expandedPolytope);
        newOrMovingNavObjectsToRefresh.insert(navObject);

        if(expandedPolytope->isWalkableCandidate())
        {
            for(std::size_t surfaceIndex=0; surfaceIndex<expandedPolytope->getSurfaces().size(); ++surfaceIndex)
            {
                const std::shared_ptr<PolytopeSurface> &polytopeSurface = expandedPolytope->getSurface(surfaceIndex);
                if(polytopeSurface->isWalkable())
                {
                    navObject->addWalkableSurface(polytopeSurface);
                }
            }
        }

        navigationObjects.addObject(new NavObjectAABBNodeData(navObject));
        aiEntity->addNavObject(navObject);
    }

    void NavMeshGenerator::removeNavObject(const std::shared_ptr<AIEntity> &aiEntity)
    {
        for(const auto &navObject : aiEntity->getNavObjects())
        {
            const std::vector<std::weak_ptr<NavObject>> &nearObjects = navObject->retrieveNearObjects();
            for(const auto &nearObject : nearObjects)
            {
                #ifndef NDEBUG
                    assert(!nearObject.expired());
                #endif
                affectedNavObjectsToRefresh.insert(nearObject.lock());
            }

            navigationObjects.removeObject(navObject);
        }
    }

    void NavMeshGenerator::prepareNavObjectsToUpdate()
    {
        ScopeProfiler scopeProfiler("ai", "prepNavObjects");

        navObjectsToRefresh.clear();
        navObjectsLinksToRefresh.clear();

        for(const auto &navObject : newOrMovingNavObjectsToRefresh)
        {
            updateNearObjects(navObject);

            for(const auto &nearObject : navObject->retrieveNearObjects())
            {
                std::shared_ptr<NavObject> sharedPtrNearObject = nearObject.lock();
                if(newOrMovingNavObjectsToRefresh.count(sharedPtrNearObject) == 0)
                {
                    affectedNavObjectsToRefresh.insert(sharedPtrNearObject);
                }
            }
        }

        for(const auto &navObject : affectedNavObjectsToRefresh)
        {
            updateNearObjects(navObject);

            //when an affected NavObject is refreshed (deleted & created): we recreate existing links toward this NavObject:
            for(const auto &relinkNavObject : navObject->retrieveNearObjects())
            {
                std::shared_ptr<NavObject> sharedPtrRelinkNavObject = relinkNavObject.lock();
                if(affectedNavObjectsToRefresh.count(sharedPtrRelinkNavObject) == 0 && newOrMovingNavObjectsToRefresh.count(sharedPtrRelinkNavObject) == 0)
                {
                    navObjectsLinksToRefresh.insert(std::make_pair(sharedPtrRelinkNavObject, navObject));
                }
            }
        }

        navObjectsToRefresh.merge(newOrMovingNavObjectsToRefresh);
        navObjectsToRefresh.merge(affectedNavObjectsToRefresh);
    }

    void NavMeshGenerator::updateNearObjects(const std::shared_ptr<NavObject> &navObject)
    {
        nearObjects.clear();
        navigationObjects.aabboxQuery(navObject->getExpandedPolytope()->getAABBox(), nearObjects);

        navObject->removeAllNearObjects();
        for (const auto &nearObject : nearObjects)
        {
            if (nearObject->getExpandedPolytope()->getName() != navObject->getExpandedPolytope()->getName())
            {
                navObject->addNearObject(nearObject);
            }
        }
    }

    void NavMeshGenerator::updateNavPolygons()
    {
        ScopeProfiler scopeProfiler("ai", "upNavPolygons");

        for(const auto &navObject : navObjectsToRefresh)
        {
            navObject->removeAllNavPolygons();
            for(const auto &walkableSurface : navObject->getWalkableSurfaces())
            {
                std::vector<std::shared_ptr<NavPolygon>> navPolygons = createNavigationPolygons(navObject, walkableSurface);
                navObject->addNavPolygons(navPolygons);
            }
        }
    }

	std::vector<std::shared_ptr<NavPolygon>> NavMeshGenerator::createNavigationPolygons(const std::shared_ptr<NavObject> &navObject,
	        const std::shared_ptr<PolytopeSurface> &walkableSurface)
	{
		ScopeProfiler scopeProfiler("ai", "createNavPolys");

        walkablePolygons.clear();
        remainingObstaclePolygons.clear();

        std::string walkableName = walkableSurface->getPolytope()->getName() + "[" + std::to_string(walkableSurface->getSurfacePosition()) + "]";
        walkablePolygons.emplace_back(CSGPolygon<float>(walkableName, walkableSurface->getOutlineCwPoints()));

        std::vector<CSGPolygon<float>> &obstaclePolygons = determineObstacles(navObject, walkableSurface);
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

	std::vector<CSGPolygon<float>> &NavMeshGenerator::determineObstacles(const std::shared_ptr<NavObject> &navObject,
	        const std::shared_ptr<PolytopeSurface> &walkableSurface) const
	{
		ScopeProfiler scopeProfiler("ai", "getObstacles");

		const std::vector<CSGPolygon<float>> &selfObstaclePolygons = walkableSurface->getSelfObstacles();

        holePolygons.clear();
        for(const auto &selfObstaclePolygon : selfObstaclePolygons)
        {
            holePolygons.emplace_back(selfObstaclePolygon);
        }

        for (const auto &nearObject : navObject->retrieveNearObjects())
        {
            const std::shared_ptr<Polytope> &nearExpandedPolytope = nearObject.lock()->getExpandedPolytope();

            if (nearExpandedPolytope->isObstacleCandidate() && nearExpandedPolytope->getAABBox().collideWithAABBox(walkableSurface->getAABBox()))
            {
                CSGPolygon<float> footprintPolygon = computePolytopeFootprint(nearExpandedPolytope, walkableSurface);
                if (footprintPolygon.getCwPoints().size() >= 3)
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
        Plane<float> walkablePlane = walkableSurface->getPlane(polytopeObstacle->getXZRectangle(), navMeshAgent);

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

	void NavMeshGenerator::subtractObstaclesOnOutline(std::vector<CSGPolygon<float>> &obstaclePolygons)
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
        TriangulationAlgorithm triangulation(std::move(walkablePolygonPoints), walkablePolygon.getName());

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
            elevatedPoints.push_back(walkableSurface->computeRealPoint(walkablePoint, navMeshAgent));
        }

		for(std::size_t holeIndex=0; holeIndex<triangulation.getHolesSize(); ++holeIndex)
		{
			const std::vector<Point2<float>> &holePoints = triangulation.getHolePoints(holeIndex);
			for(const auto &holePoint : holePoints)
			{
				elevatedPoints.push_back(walkableSurface->computeRealPoint(holePoint, navMeshAgent));
			}
		}

		return elevatedPoints;
	}

    void NavMeshGenerator::deleteNavLinks()
    {
        ScopeProfiler scopeProfiler("ai", "delNavLinks");

        for(const auto &navObjectLinksToRefresh : navObjectsLinksToRefresh)
        {
            for (const auto &sourceNavPolygon : navObjectLinksToRefresh.first->getNavPolygons())
            {
                for (const auto &targetNavPolygon : navObjectLinksToRefresh.second->getNavPolygons())
                {
                    sourceNavPolygon->removeLinksTo(targetNavPolygon);
                }
            }
        }
    }

	void NavMeshGenerator::createNavLinks()
    {
        ScopeProfiler scopeProfiler("ai", "creNavLinks");

        for(const auto &sourceNavObject : navObjectsToRefresh)
        {
            for(const auto &sourceNavPolygon : sourceNavObject->getNavPolygons())
            {
                for(const auto &sourceExternalEdge : sourceNavPolygon->retrieveExternalEdges())
                {
                    for(const auto &targetNavObject : sourceNavObject->retrieveNearObjects())
                    {
                        createNavLinks(sourceExternalEdge, targetNavObject.lock());
                    }
                }
            }
        }

        for(const auto &navObjectLinksToRefresh : navObjectsLinksToRefresh)
        {
            for(const auto &sourceNavPolygon : navObjectLinksToRefresh.first->getNavPolygons())
            {
                for (const auto &sourceExternalEdge : sourceNavPolygon->retrieveExternalEdges())
                {
                    createNavLinks(sourceExternalEdge, navObjectLinksToRefresh.second);
                }
            }
        }
    }

    void NavMeshGenerator::createNavLinks(const NavPolygonEdge &sourceExternalEdge, const std::shared_ptr<NavObject> &targetNavObject) const
    {
        EdgeJumpDetection edgeJumpDetection(navMeshAgent->getJumpDistance());
        LineSegment3D<float> sourceEdge = sourceExternalEdge.triangle->computeEdge(sourceExternalEdge.edgeIndex);

        for(const auto &targetNavPolygon : targetNavObject->getNavPolygons())
        {
            for(const auto &targetExternalEdge : targetNavPolygon->retrieveExternalEdges())
            {
                LineSegment3D<float> targetEdge = targetExternalEdge.triangle->computeEdge(targetExternalEdge.edgeIndex);

                EdgeJumpResult edgeJumpResult = edgeJumpDetection.detectJump(sourceEdge, targetEdge);
                if (edgeJumpResult.hasJumpRange())
                {
                    auto *navJumpConstraint = new NavJumpConstraint(edgeJumpResult.getJumpStartRange(), edgeJumpResult.getJumpEndRange(), targetExternalEdge.edgeIndex);
                    sourceExternalEdge.triangle->addJumpLink(sourceExternalEdge.edgeIndex, targetExternalEdge.triangle, navJumpConstraint);
                }
            }
        }
    }

    void NavMeshGenerator::updateNavMesh()
    {
        allNavPolygons.clear();

        allNavObjects.clear();
        navigationObjects.getAllNodeObjects(allNavObjects);

        for(const auto &navObject : allNavObjects)
        {
            const std::vector<std::shared_ptr<NavPolygon>> &navPolygons = navObject->getNavPolygons();
            allNavPolygons.insert(allNavPolygons.end(), navPolygons.begin(), navPolygons.end());
        }

        std::lock_guard<std::mutex> lock(navMeshMutex);
        navMesh->copyAllPolygons(allNavPolygons);
    }

}
