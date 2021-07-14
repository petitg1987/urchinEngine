#include <cmath>
#include <algorithm>
#include <string>
#include <numeric>

#include <path/navmesh/NavMeshGenerator.h>
#include <path/navmesh/polytope/PolytopePlaneSurface.h>
#include <path/navmesh/polytope/PolytopeBuilder.h>
#include <path/navmesh/polytope/aabbtree/NavObjectAABBNodeData.h>
#include <path/navmesh/csg/PolygonsUnion.h>
#include <path/navmesh/csg/PolygonsSubtraction.h>
#include <path/navmesh/link/EdgeLinkDetection.h>
#include <input/AIObject.h>
#include <input/AITerrain.h>

namespace urchin {

    //debug parameters
    bool DEBUG_EXPORT_NAV_MESH = false;

    NavMeshGenerator::NavMeshGenerator() :
            polygonMinDotProductThreshold(std::cos(AngleConverter<float>::toRadian(ConfigService::instance()->getFloatValue("navMesh.polygonRemoveAngleThresholdInDegree")))),
            polygonMergePointsDistanceThreshold(ConfigService::instance()->getFloatValue("navMesh.polygonMergePointsDistanceThreshold")),
            navMeshAgent(std::make_unique<NavMeshAgent>()),
            navMesh(std::make_shared<NavMesh>()),
            needFullRefresh(false),
            navigationObjects(AABBTree<std::shared_ptr<NavObject>>(ConfigService::instance()->getFloatValue("navMesh.polytopeAabbTreeFatMargin"))) {

    }

    void NavMeshGenerator::setNavMeshAgent(std::unique_ptr<NavMeshAgent> navMeshAgent) {
        std::lock_guard<std::mutex> lock(navMeshMutex);

        this->navMeshAgent = std::move(navMeshAgent);
        this->needFullRefresh.store(true, std::memory_order_release);

        float navigationObjectsJumpMargin = this->navMeshAgent->getJumpDistance() / 2.0f;
        float navigationObjectsMargin = std::max(navigationObjectsJumpMargin, ConfigService::instance()->getFloatValue("navMesh.polytopeAabbTreeFatMargin"));
        this->navigationObjects.updateFatMargin(navigationObjectsMargin);
    }

    const NavMeshAgent* NavMeshGenerator::getNavMeshAgent() const {
        return navMeshAgent.get();
    }

    NavMesh NavMeshGenerator::copyLastGeneratedNavMesh() const {
        std::lock_guard<std::mutex> lock(navMeshMutex);

        return NavMesh(*navMesh);
    }

    /**
     * See '_doc' for an algorithm overview
     */
    std::shared_ptr<NavMesh> NavMeshGenerator::generate(AIWorld& aiWorld) {
        ScopeProfiler sp(Profiler::ai(), "navMeshGenerate");

        updateExpandedPolytopes(aiWorld);
        prepareNavObjectsToUpdate();
        deleteNavLinks();
        updateNavPolygons();
        createNavLinks();
        updateNavMesh();

        if (DEBUG_EXPORT_NAV_MESH) {
            navMesh->svgMeshExport(SystemInfo::homeDirectory() + "navMesh/navMesh" + std::to_string(navMesh->getUpdateId()) + ".html");
        }

        return navMesh;
    }

    void NavMeshGenerator::updateExpandedPolytopes(AIWorld& aiWorld) {
        assert(navMeshAgent);
        ScopeProfiler sp(Profiler::ai(), "upExpandPoly");

        newOrMovingNavObjectsToRefresh.clear();
        affectedNavObjectsToRefresh.clear();

        for (auto& aiObjectToRemove : aiWorld.getEntitiesToRemoveAndReset()) {
            removeNavObject(aiObjectToRemove);
        }

        bool refreshAllEntities = needFullRefresh.exchange(false, std::memory_order_release);
        for (auto& aiEntity : aiWorld.getEntities()) {
            if (aiEntity->isToRebuild() || refreshAllEntities) {
                removeNavObject(aiEntity);
                aiEntity->removeAllNavObjects();

                if (aiEntity->getType() == AIEntity::OBJECT) {
                    auto aiObject = std::dynamic_pointer_cast<AIObject>(aiEntity);
                    std::vector<std::unique_ptr<Polytope>> objectExpandedPolytopes = PolytopeBuilder::instance()->buildExpandedPolytopes(aiObject, *navMeshAgent);
                    for (auto& objectExpandedPolytope : objectExpandedPolytopes) {
                        addNavObject(aiObject, std::move(objectExpandedPolytope));
                    }
                } else if (aiEntity->getType() == AIEntity::TERRAIN) {
                    auto aiTerrain = std::dynamic_pointer_cast<AITerrain>(aiEntity);
                    std::vector<std::unique_ptr<Polytope>> terrainExpandedPolytopes = PolytopeBuilder::instance()->buildExpandedPolytope(aiTerrain, *navMeshAgent);
                    for (auto& terrainExpandedPolytope : terrainExpandedPolytopes) {
                        addNavObject(aiTerrain, std::move(terrainExpandedPolytope));
                    }
                }

                aiEntity->markRebuilt();
            }
        }
    }

    void NavMeshGenerator::addNavObject(const std::shared_ptr<AIEntity>& aiEntity, std::unique_ptr<Polytope>&& expandedPolytope) {
        auto navObject = std::make_shared<NavObject>(std::move(expandedPolytope));
        newOrMovingNavObjectsToRefresh.insert(navObject);

        if (navObject->getExpandedPolytope()->isWalkableCandidate()) {
            for (std::size_t surfaceIndex = 0; surfaceIndex < navObject->getExpandedPolytope()->getSurfaces().size(); ++surfaceIndex) {
                const std::shared_ptr<PolytopeSurface>& polytopeSurface = navObject->getExpandedPolytope()->getSurface(surfaceIndex);
                if (polytopeSurface->isWalkable()) {
                    navObject->addWalkableSurface(polytopeSurface);
                }
            }
        }

        navigationObjects.addObject(std::make_unique<NavObjectAABBNodeData>(navObject));
        aiEntity->addNavObject(navObject);
    }

    void NavMeshGenerator::removeNavObject(const std::shared_ptr<AIEntity>& aiEntity) {
        for (const auto& navObject : aiEntity->getNavObjects()) {
            const std::vector<std::weak_ptr<NavObject>>& nearObjects = navObject->retrieveNearObjects();
            for (const auto& nearObject : nearObjects) {
                #ifndef NDEBUG
                    assert(!nearObject.expired());
                #endif
                affectedNavObjectsToRefresh.insert(nearObject.lock());
            }

            navigationObjects.removeObject(navObject);
        }
    }

    void NavMeshGenerator::prepareNavObjectsToUpdate() {
        ScopeProfiler sp(Profiler::ai(), "prepNavObjects");

        navObjectsToRefresh.clear();
        navObjectsLinksToRefresh.clear();

        for (const auto& navObject : newOrMovingNavObjectsToRefresh) {
            updateNearObjects(navObject);

            for (const auto& nearObject : navObject->retrieveNearObjects()) {
                std::shared_ptr<NavObject> sharedPtrNearObject = nearObject.lock();
                if (newOrMovingNavObjectsToRefresh.count(sharedPtrNearObject) == 0) {
                    affectedNavObjectsToRefresh.insert(sharedPtrNearObject);
                }
            }
        }

        for (const auto& navObject : affectedNavObjectsToRefresh) {
            updateNearObjects(navObject);

            //when an affected NavObject is refreshed (deleted & created): we recreate existing links toward this NavObject:
            for (const auto& relinkNavObject : navObject->retrieveNearObjects()) {
                std::shared_ptr<NavObject> sharedPtrRelinkNavObject = relinkNavObject.lock();
                if (affectedNavObjectsToRefresh.count(sharedPtrRelinkNavObject) == 0 && newOrMovingNavObjectsToRefresh.count(sharedPtrRelinkNavObject) == 0) {
                    navObjectsLinksToRefresh.insert(std::make_pair(sharedPtrRelinkNavObject, navObject));
                }
            }
        }

        navObjectsToRefresh.merge(newOrMovingNavObjectsToRefresh);
        navObjectsToRefresh.merge(affectedNavObjectsToRefresh);
    }

    void NavMeshGenerator::updateNearObjects(const std::shared_ptr<NavObject>& navObject) {
        nearObjects.clear();
        navigationObjects.aabboxQuery(navObject->getExpandedPolytope()->getAABBox(), nearObjects);

        navObject->removeAllNearObjects();
        for (const auto& nearObject : nearObjects) {
            if (nearObject->getExpandedPolytope()->getName() != navObject->getExpandedPolytope()->getName()) {
                navObject->addNearObject(nearObject);
            }
        }
    }

    void NavMeshGenerator::updateNavPolygons() {
        ScopeProfiler sp(Profiler::ai(), "upNavPolygons");

        for (const auto& navObject : navObjectsToRefresh) {
            navObject->removeAllNavPolygons();
            for (const auto& walkableSurface : navObject->getWalkableSurfaces()) {
                std::vector<std::shared_ptr<NavPolygon>> navPolygons = createNavigationPolygons(navObject, walkableSurface);
                navObject->addNavPolygons(navPolygons);
            }
        }
    }

    std::vector<std::shared_ptr<NavPolygon>> NavMeshGenerator::createNavigationPolygons(const std::shared_ptr<NavObject>& navObject,
            const std::shared_ptr<PolytopeSurface>& walkableSurface) {
        ScopeProfiler sp(Profiler::ai(), "createNavPolys");

        std::string walkableName = walkableSurface->getPolytope()->getName() + "[" + std::to_string(walkableSurface->getSurfacePosition()) + "]";
        walkablePolygons.clear();
        walkablePolygons.emplace_back(CSGPolygon<float>(walkableName, walkableSurface->getOutlineCwPoints()));
        std::vector<CSGPolygon<float>>& obstaclePolygons = determineObstacles(navObject, walkableSurface);

        applyObstaclesOnWalkablePolygon(obstaclePolygons);

        bool uniqueWalkableSurface = walkablePolygons.size() == 1;
        std::vector<std::shared_ptr<NavPolygon>> navPolygons;
        navPolygons.reserve(walkablePolygons.size());
        for (auto& walkablePolygon : walkablePolygons) {
            //simplify polygon to improve performance and avoid degenerated walkable face
            walkablePolygon.simplify(polygonMinDotProductThreshold, polygonMergePointsDistanceThreshold);
            if (walkablePolygon.getCwPoints().size() > 2) {
                std::shared_ptr<NavPolygon> navPolygon = createNavigationPolygon(walkablePolygon, walkableSurface, uniqueWalkableSurface);
                navPolygons.push_back(navPolygon);
            }
        }

        return navPolygons;
    }

    std::vector<CSGPolygon<float>>& NavMeshGenerator::determineObstacles(const std::shared_ptr<NavObject>& navObject,
            const std::shared_ptr<PolytopeSurface>& walkableSurface) const {
        ScopeProfiler sp(Profiler::ai(), "getObstacles");

        const std::vector<CSGPolygon<float>>& selfObstaclePolygons = walkableSurface->getSelfObstacles();

        holePolygons.clear();
        for (const auto& selfObstaclePolygon : selfObstaclePolygons) {
            holePolygons.emplace_back(selfObstaclePolygon);
        }

        for (const auto& nearObject : navObject->retrieveNearObjects()) {
            const std::shared_ptr<Polytope>& nearExpandedPolytope = nearObject.lock()->getExpandedPolytope();

            if (nearExpandedPolytope->isObstacleCandidate() && nearExpandedPolytope->getAABBox().collideWithAABBox(walkableSurface->getAABBox())) {
                CSGPolygon<float> footprintPolygon = computePolytopeFootprint(nearExpandedPolytope, walkableSurface);
                if (footprintPolygon.getCwPoints().size() >= 3) {
                    footprintPolygon.simplify(polygonMinDotProductThreshold, polygonMergePointsDistanceThreshold);
                    holePolygons.push_back(std::move(footprintPolygon));
                }
            }
        }

        return PolygonsUnion<float>::instance()->unionPolygons(holePolygons);
    }

    CSGPolygon<float> NavMeshGenerator::computePolytopeFootprint(const std::shared_ptr<Polytope>& polytopeObstacle, const std::shared_ptr<PolytopeSurface>& walkableSurface) const {
        footprintPoints.clear();
        Plane<float> walkablePlane = walkableSurface->getPlane(polytopeObstacle->getXZRectangle());

        for (const auto& polytopeSurface : polytopeObstacle->getSurfaces()) {
            if (auto* polytopePlaneSurface = dynamic_cast<PolytopePlaneSurface*>(polytopeSurface.get())) {
                for (std::size_t i = 0, previousI = polytopePlaneSurface->getCcwPoints().size() - 1; i < polytopePlaneSurface->getCcwPoints().size(); previousI = i++) {
                    float distance1 = walkablePlane.distance(polytopePlaneSurface->getCcwPoints()[previousI]);
                    float distance2 = walkablePlane.distance(polytopePlaneSurface->getCcwPoints()[i]);
                    if (std::signbit(distance1) != std::signbit(distance2)) {
                        Line3D<float> polytopeEdgeLine(polytopePlaneSurface->getCcwPoints()[previousI], polytopePlaneSurface->getCcwPoints()[i]);
                        bool hasIntersection = false;
                        Point3<float> intersectionPoint = walkablePlane.intersectPoint(polytopeEdgeLine, hasIntersection);
                        footprintPoints.emplace_back(Point2<float>(intersectionPoint.X, -intersectionPoint.Z));
                    }
                }
            } else {
                throw std::runtime_error("Unsupported type of surface as obstacle: " + std::string(typeid(polytopeSurface.get()).name()));
            }
        }

        if (footprintPoints.size() < 3) { //no footprint: return empty polygon
            return CSGPolygon<float>("", {});
        }

        ConvexHull2D<float> footprintConvexHull(footprintPoints);
        std::vector<Point2<float>> cwPoints(footprintConvexHull.getPoints());
        std::reverse(cwPoints.begin(), cwPoints.end());
        return CSGPolygon<float>(polytopeObstacle->getName(), std::move(cwPoints));
    }

    void NavMeshGenerator::applyObstaclesOnWalkablePolygon(std::vector<CSGPolygon<float>>& obstaclePolygons) {
        ScopeProfiler sp(Profiler::ai(), "subObstacles");

        assert(walkablePolygons.size() == 1);
        obstaclesInsideWalkablePolygon.clear();

        for (auto& obstaclePolygon : obstaclePolygons) {
            obstaclePolygon.simplify(polygonMinDotProductThreshold, polygonMergePointsDistanceThreshold);
            if (obstaclePolygon.getCwPoints().size() > 2) {
                auto walkablePolygonsCounter = (int)walkablePolygons.size();
                while (walkablePolygonsCounter-- != 0) {
                    const CSGPolygon<float>& walkablePolygon = walkablePolygons[0];

                    bool obstacleInsideWalkable;
                    const std::vector<CSGPolygon<float>>& subtractedPolygons = PolygonsSubtraction<float>::instance()->subtractPolygons(
                            walkablePolygon, obstaclePolygon, obstacleInsideWalkable);

                    //replace 'walkablePolygon' by 'subtractedPolygons'
                    walkablePolygons.erase(walkablePolygons.begin());
                    for (auto& subtractedPolygon : subtractedPolygons) {
                        walkablePolygons.emplace_back(subtractedPolygon);
                    }

                    if (obstacleInsideWalkable) {
                        obstaclesInsideWalkablePolygon.emplace_back(obstaclePolygon);
                        break;
                    }
                }
            }
        }
    }

    std::shared_ptr<NavPolygon> NavMeshGenerator::createNavigationPolygon(CSGPolygon<float>& walkablePolygon, const std::shared_ptr<PolytopeSurface>& walkableSurface, bool uniqueWalkableSurface) const {
        ScopeProfiler sp(Profiler::ai(), "createNavPoly");

        navPolygonName = "<" + walkablePolygon.getName() + ">";
        std::vector<std::vector<Point2<float>>> polygonPoints;
        polygonPoints.reserve(walkablePolygon.getCwPoints().size() + obstaclesInsideWalkablePolygon.size());
        polygonPoints.emplace_back(walkablePolygon.getCwPoints());
        for (const auto& obstacleInsideWalkablePolygon : obstaclesInsideWalkablePolygon) {
            if (uniqueWalkableSurface || walkablePolygon.pointInsideOrOnPolygon(obstacleInsideWalkablePolygon.getCwPoints()[0])) { //obstacle fully inside walkable polygon
                polygonPoints.emplace_back(obstacleInsideWalkablePolygon.getCwPoints());
                navPolygonName += " - <" + obstacleInsideWalkablePolygon.getName() + ">";
            }
        }

        std::vector<Point3<float>> elevatedPoints = elevateTriangulatedPoints(polygonPoints, walkableSurface);
        auto navPolygon = std::make_shared<NavPolygon>(navPolygonName, std::move(elevatedPoints), walkableSurface->getNavTopography());
        TriangulationAlgorithm triangulation(std::move(polygonPoints));
        navPolygon->addTriangles(triangulation.triangulate(), navPolygon);

        return navPolygon;
    }

    std::vector<Point3<float>> NavMeshGenerator::elevateTriangulatedPoints(const std::vector<std::vector<Point2<float>>>& polygonPoints, const std::shared_ptr<PolytopeSurface>& walkableSurface) const {
        ScopeProfiler sp(Profiler::ai(), "elevateTriPoint");

        std::vector<Point3<float>> elevatedPoints;
        std::size_t polygonPointsCount = 0;
        std::for_each(polygonPoints.begin(), polygonPoints.end(),[&polygonPointsCount](const std::vector<Point2<float>>& vec) {polygonPointsCount += vec.size();});
        elevatedPoints.reserve(polygonPointsCount);

        for (const auto& polygonComponent : polygonPoints) {
            for (const auto& polygonPoint : polygonComponent) {
                elevatedPoints.push_back(walkableSurface->computeRealPoint(polygonPoint, *navMeshAgent));
            }
        }

        return elevatedPoints;
    }

    void NavMeshGenerator::deleteNavLinks() {
        ScopeProfiler sp(Profiler::ai(), "delNavLinks");

        for (const auto& navObjectLinksToRefresh : navObjectsLinksToRefresh) {
            for (const auto& sourceNavPolygon : navObjectLinksToRefresh.first->getNavPolygons()) {
                for (const auto& targetNavPolygon : navObjectLinksToRefresh.second->getNavPolygons()) {
                    sourceNavPolygon->removeLinksTo(targetNavPolygon);
                }
            }
        }
    }

    void NavMeshGenerator::createNavLinks() {
        ScopeProfiler sp(Profiler::ai(), "creNavLinks");

        for (const auto& sourceNavObject : navObjectsToRefresh) {
            for (const auto& sourceNavPolygon : sourceNavObject->getNavPolygons()) {
                for (const auto& sourceExternalEdge : sourceNavPolygon->retrieveExternalEdges()) {
                    for (const auto& targetNavObject : sourceNavObject->retrieveNearObjects()) {
                        createNavLinks(sourceExternalEdge, targetNavObject.lock());
                    }
                }
            }
        }

        for (const auto& navObjectLinksToRefresh : navObjectsLinksToRefresh) {
            for (const auto& sourceNavPolygon : navObjectLinksToRefresh.first->getNavPolygons()) {
                for (const auto& sourceExternalEdge : sourceNavPolygon->retrieveExternalEdges()) {
                    createNavLinks(sourceExternalEdge, navObjectLinksToRefresh.second);
                }
            }
        }
    }

    void NavMeshGenerator::createNavLinks(const NavPolygonEdge& sourceExternalEdge, const std::shared_ptr<NavObject>& targetNavObject) const {
        EdgeLinkDetection edgeLinkDetection(navMeshAgent->getJumpDistance());
        LineSegment3D<float> sourceEdge = sourceExternalEdge.triangle->computeEdge(sourceExternalEdge.edgeIndex);

        for (const auto& targetNavPolygon : targetNavObject->getNavPolygons()) {
            for (const auto& targetExternalEdge : targetNavPolygon->retrieveExternalEdges()) {
                LineSegment3D<float> targetEdge = targetExternalEdge.triangle->computeEdge(targetExternalEdge.edgeIndex);

                EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(sourceEdge, targetEdge);
                if (edgeLinkResult.hasEdgesLink()) {
                    auto navLinkConstraint = std::make_unique<NavLinkConstraint>(edgeLinkResult.getLinkStartRange(), edgeLinkResult.getLinkEndRange(), targetExternalEdge.edgeIndex);
                    if (edgeLinkResult.isJumpLink()) {
                        sourceExternalEdge.triangle->addJumpLink(sourceExternalEdge.edgeIndex, targetExternalEdge.triangle, std::move(navLinkConstraint));
                    } else {
                        sourceExternalEdge.triangle->addJoinPolygonsLink(sourceExternalEdge.edgeIndex, targetExternalEdge.triangle, std::move(navLinkConstraint));
                    }
                }
            }
        }
    }

    void NavMeshGenerator::updateNavMesh() {
        allNavPolygons.clear();

        allNavObjects.clear();
        navigationObjects.getAllNodeObjects(allNavObjects);

        for (const auto& navObject : allNavObjects) {
            const std::vector<std::shared_ptr<NavPolygon>>& navPolygons = navObject->getNavPolygons();
            allNavPolygons.insert(allNavPolygons.end(), navPolygons.begin(), navPolygons.end());
        }

        std::lock_guard<std::mutex> lock(navMeshMutex);
        navMesh->copyAllPolygons(allNavPolygons);
    }

}
