#include <cassert>
#include <ranges>
#include <UrchinCommon.h>

#include <path/navmesh/polytope/PolytopeBuilder.h>
#include <path/navmesh/polytope/PolytopePlaneSurface.h>
#include <path/navmesh/polytope/PolytopeTerrainSurface.h>
#include <path/navmesh/polytope/services/TerrainObstacleService.h>
#include <path/navmesh/model/output/topography/NavTerrainTopography.h>

namespace urchin {

    PolytopeBuilder& PolytopeBuilder::instance() {
        static PolytopeBuilder instance;
        return instance;
    }

    PolytopeBuilder::PolytopeBuilder() :
            planeSurfaceSplitService(PlaneSurfaceSplitService(ConfigService::instance().getFloatValue("navMesh.polytopeMaxSize"))),
            terrainSplitService(TerrainSplitService(ConfigService::instance().getFloatValue("navMesh.polytopeMaxSize"))) {

    }

    std::vector<std::unique_ptr<Polytope>> PolytopeBuilder::buildExpandedPolytopes(const AIObject& aiObject, const NavMeshAgent& navMeshAgent) const {
        std::vector<std::unique_ptr<Polytope>> expandedPolytopes;

        unsigned int aiShapeIndex = 0;
        for (auto& aiShape : aiObject.getShapes()) {
            std::string shapeName = aiObject.getShapes().size() == 1 ? aiObject.getName() : aiObject.getName() + "[" + std::to_string(aiShapeIndex++) + "]";
            Transform<float> shapeTransform = aiShape->hasLocalTransform() ? aiObject.getTransform() * aiShape->getLocalTransform() : aiObject.getTransform();
            std::unique_ptr<ConvexObject3D<float>> object = aiShape->getShape().toConvexObject(shapeTransform);
            std::unique_ptr<Polytope> expandedPolytope;

            if (object->getObjectType() == ConvexObjectType::OBBOX) {
                expandedPolytope = createExpandedPolytope(shapeName, static_cast<OBBox<float>*>(object.get()), navMeshAgent);
            } else if (object->getObjectType() == ConvexObjectType::CAPSULE) {
                expandedPolytope = createExpandedPolytope(shapeName, static_cast<Capsule<float>*>(object.get()), navMeshAgent);
            } else if (object->getObjectType() == ConvexObjectType::CONE) {
                expandedPolytope = createExpandedPolytope(shapeName, static_cast<Cone<float>*>(object.get()), navMeshAgent);
            } else if (object->getObjectType() == ConvexObjectType::CONVEX_HULL) {
                expandedPolytope = createExpandedPolytope(shapeName, static_cast<ConvexHull3D<float>*>(object.get()), navMeshAgent);
            } else if (object->getObjectType() == ConvexObjectType::CYLINDER) {
                expandedPolytope = createExpandedPolytope(shapeName, static_cast<Cylinder<float>*>(object.get()), navMeshAgent);
            } else if (object->getObjectType() == ConvexObjectType::SPHERE) {
                expandedPolytope = createExpandedPolytope(shapeName, static_cast<Sphere<float>*>(object.get()), navMeshAgent);
            } else {
                throw std::invalid_argument("Shape type " + std::to_string((int)object->getObjectType()) + " not supported by navigation mesh generator for object: " + aiObject.getName());
            }

            expandedPolytope->setObstacleCandidate(aiObject.isObstacleCandidate());
            expandedPolytopes.push_back(std::move(expandedPolytope));
        }

        return expandedPolytopes;
    }

    std::vector<std::unique_ptr<Polytope>> PolytopeBuilder::buildExpandedPolytope(const AITerrain& aiTerrain, const NavMeshAgent& navMeshAgent) const {
        #ifdef URCHIN_DEBUG
            assert(!aiTerrain.getTransform().hasScaling());
            assert(MathFunction::isOne(aiTerrain.getTransform().getOrientationMatrix().determinant()));
        #endif

        std::vector<std::unique_ptr<Polytope>> expandedPolytopes;

        auto terrainMaxWalkableSlope = AngleConverter<float>::toRadian(ConfigService::instance().getFloatValue("navMesh.terrainMaxWalkableSlopeInDegree"));
        auto heightfieldPointHelper = std::make_unique<const HeightfieldPointHelper<float>>(aiTerrain.getLocalVertices(), aiTerrain.getXLength());
        auto terrainNavTopography = std::make_shared<NavTerrainTopography>(std::move(heightfieldPointHelper), aiTerrain.getTransform().getPosition());

        Vector3 approximateNormal(0.0f, 1.0f, 0.0f); //use approximate normal for all terrain surface instead of normal by vertex to speed up the computation
        Vector3<float> expandShiftVector = approximateNormal * navMeshAgent.computeExpandDistance(approximateNormal);
        std::vector<Point3<float>> expandedLocalVertices;
        expandedLocalVertices.reserve(aiTerrain.getLocalVertices().size());
        for (const auto& localVertex : aiTerrain.getLocalVertices()) {
            expandedLocalVertices.emplace_back(localVertex.translate(expandShiftVector));
        }

        std::vector<TerrainSplit> terrainSplits = terrainSplitService.splitTerrain(aiTerrain.getName(), aiTerrain.getTransform().getPosition(),
                expandedLocalVertices, aiTerrain.getXLength(), aiTerrain.getZLength());

        for (const auto& terrainSplit : terrainSplits) {
            TerrainObstacleService terrainObstacleService(terrainSplit.name, terrainSplit.position, terrainSplit.localVertices, terrainSplit.xLength, terrainSplit.zLength);
            std::vector<CSGPolygon<float>> selfObstacles = terrainObstacleService.computeSelfObstacles(terrainMaxWalkableSlope);

            auto terrainSurface = std::make_shared<PolytopeTerrainSurface>(terrainSplit.position, terrainSplit.localVertices, terrainSplit.xLength, terrainSplit.zLength,
                    approximateNormal, selfObstacles, terrainNavTopography);
            terrainSurface->setWalkableCandidate(true);
            std::vector<std::shared_ptr<PolytopeSurface>> expandedSurfaces;
            expandedSurfaces.emplace_back(std::move(terrainSurface));

            auto expandedPolytope = std::make_unique<Polytope>(terrainSplit.name, expandedSurfaces);
            expandedPolytope->setWalkableCandidate(true);
            expandedPolytope->setObstacleCandidate(aiTerrain.isObstacleCandidate());
            expandedPolytopes.push_back(std::move(expandedPolytope));
        }

        return expandedPolytopes;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytope(std::string name, const OBBox<float>* box, const NavMeshAgent& navMeshAgent) const {
        std::vector<Point3<float>> sortedOriginalPoints = box->getPoints();
        std::vector<Point3<float>> sortedExpandedPoints = createExpandedPoints(sortedOriginalPoints, navMeshAgent);
        std::vector<std::shared_ptr<PolytopeSurface>> expandedPolytopeSurfaces = createExpandedPolytopeSurfaces(sortedOriginalPoints, sortedExpandedPoints, navMeshAgent);
        return std::make_unique<Polytope>(std::move(name), expandedPolytopeSurfaces);
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytope(std::string name, const Capsule<float>* capsule, const NavMeshAgent& navMeshAgent) const {
        Vector3 boxHalfSizes(capsule->getRadius(), capsule->getRadius(), capsule->getRadius());
        boxHalfSizes[capsule->getCapsuleOrientation()] += capsule->getCylinderHeight() / 2.0f;

        OBBox capsuleBox(boxHalfSizes, capsule->getCenterOfMass(), capsule->getOrientation());
        std::unique_ptr<Polytope> polytope = createExpandedPolytope(std::move(name), &capsuleBox, navMeshAgent);
        polytope->setWalkableCandidate(false);

        return polytope;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytope(std::string name, const Cone<float>* cone, const NavMeshAgent& navMeshAgent) const {
        Vector3 boxHalfSizes(cone->getRadius(), cone->getRadius(), cone->getRadius());
        boxHalfSizes[cone->getConeOrientation() / 2] = cone->getHeight() / 2.0f;

        OBBox coneBox(boxHalfSizes, cone->getCenter(), cone->getOrientation());
        std::unique_ptr<Polytope> polytope = createExpandedPolytope(std::move(name), &coneBox, navMeshAgent);
        polytope->setWalkableCandidate(false);

        return polytope;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytope(std::string name, const ConvexHull3D<float>* convexHull, const NavMeshAgent& navMeshAgent) const {
        std::map<std::size_t, Plane<float>> expandedPlanes;
        for (const auto& [triangleId, triangle] : convexHull->getIndexedTriangles()) {
            const Point3<float>& point1 = convexHull->getConvexHullPoints().at(triangle.getIndex(0)).point;
            const Point3<float>& point2 = convexHull->getConvexHullPoints().at(triangle.getIndex(1)).point;
            const Point3<float>& point3 = convexHull->getConvexHullPoints().at(triangle.getIndex(2)).point;
            expandedPlanes.try_emplace(triangleId, createExpandedPlane(point1, point2, point3, navMeshAgent));
        }

        std::unique_ptr<ConvexHull3D<float>> expandedConvexHull = ResizeConvexHull3DService<float>::resizeConvexHull(*convexHull, expandedPlanes);

        std::vector<std::shared_ptr<PolytopeSurface>> expandedSurfaces;
        expandedSurfaces.reserve(expandedConvexHull->getIndexedTriangles().size() * 3);
        for (const IndexedTriangle3D<float>& triangle : std::views::values(expandedConvexHull->getIndexedTriangles())) {
            const std::array<std::size_t, 3>& indices = triangle.getIndices();

            std::vector surfacePoints = {
                    expandedConvexHull->getConvexHullPoints().find(indices[0])->second.point,
                    expandedConvexHull->getConvexHullPoints().find(indices[1])->second.point,
                    expandedConvexHull->getConvexHullPoints().find(indices[2])->second.point};

            expandedSurfaces.emplace_back(std::make_shared<PolytopePlaneSurface>(std::move(surfacePoints), true));
        }

        auto polytope = std::make_unique<Polytope>(std::move(name), expandedSurfaces);
        polytope->setWalkableCandidate(false);

        return polytope;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytope(std::string name, const Cylinder<float>* cylinder, const NavMeshAgent& navMeshAgent) const {
        Vector3 boxHalfSizes(cylinder->getRadius(), cylinder->getRadius(), cylinder->getRadius());
        boxHalfSizes[cylinder->getCylinderOrientation()] = cylinder->getHeight() / 2.0f;

        OBBox cylinderBox(boxHalfSizes, cylinder->getCenterOfMass(), cylinder->getOrientation());
        std::vector<Point3<float>> sortedOriginalPoints = cylinderBox.getPoints();
        std::vector<Point3<float>> sortedExpandedPoints = createExpandedPoints(sortedOriginalPoints, navMeshAgent);

        std::vector<std::shared_ptr<PolytopeSurface>> expandedSurfaces = createExpandedPolytopeSurfaces(sortedOriginalPoints, sortedExpandedPoints, navMeshAgent);

        for (std::size_t i = 0; i < expandedSurfaces.size(); ++i) {
            expandedSurfaces[i]->setWalkableCandidate(cylinder->getCylinderOrientation() == i / 2);
        }

        return std::make_unique<Polytope>(std::move(name), expandedSurfaces);
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytope(std::string name, const Sphere<float>* sphere, const NavMeshAgent& navMeshAgent) const {
        OBBox sphereBox(*sphere);
        std::unique_ptr<Polytope> polytope = createExpandedPolytope(std::move(name), &sphereBox, navMeshAgent);
        polytope->setWalkableCandidate(false);

        return polytope;
    }

    /**
     * Return box points. Points are in the following order: NTR, FTR, NBR, FBR, NTL, FTL, NBL, FBL
     */
    std::vector<Point3<float>> PolytopeBuilder::createExpandedPoints(const std::vector<Point3<float>>& sortedPoints, const NavMeshAgent& navMeshAgent) const {
        std::vector<Plane<float>> sortedExpandedPlanes = createExpandedBoxPlanes(sortedPoints, navMeshAgent);
        return expandBoxPoints(sortedExpandedPlanes);
    }

    /**
     * Returns expanded planes.
     * @param sortedPoints Points in the following order: NTR, FTR, NBR, FBR, NTL, FTL, NBL, FBL
     * @return Expanded planes in the following order: right, left, top, bottom, front, back
     */
    std::vector<Plane<float>> PolytopeBuilder::createExpandedBoxPlanes(const std::vector<Point3<float>>& sortedPoints, const NavMeshAgent& navMeshAgent) const {
        std::vector<Plane<float>> expandedPlanes;
        expandedPlanes.reserve(6);

        for (const auto& pointIndex : POINT_INDEX_TO_PLANES) {
            expandedPlanes.emplace_back(createExpandedPlane(sortedPoints[pointIndex[0]], sortedPoints[pointIndex[1]], sortedPoints[pointIndex[2]], navMeshAgent));
        }

        return expandedPlanes;
    }

    Plane<float> PolytopeBuilder::createExpandedPlane(const Point3<float>& p1, const Point3<float>& p2, const Point3<float>& p3, const NavMeshAgent& navMeshAgent) const {
        Plane plane(p1, p2, p3);

        float expandDistance = navMeshAgent.computeExpandDistance(plane.getNormal());
        plane.setDistanceToOrigin(plane.getDistanceToOrigin() - expandDistance);

        return plane;
    }

    std::vector<Point3<float>> PolytopeBuilder::expandBoxPoints(const std::vector<Plane<float>>& sortedExpandedPlanes) const {
        std::vector<Point3<float>> expandedPoints;
        expandedPoints.reserve(8);

        for (auto planeIndex : PLANE_INDEX_TO_POINTS) {
            const Plane<float>& plane0 = sortedExpandedPlanes[planeIndex[0]];
            const Plane<float>& plane1 = sortedExpandedPlanes[planeIndex[1]];
            const Plane<float>& plane2 = sortedExpandedPlanes[planeIndex[2]];

            Vector3<float> n1CrossN2 = plane0.getNormal().crossProduct(plane1.getNormal());
            Vector3<float> n2CrossN3 = plane1.getNormal().crossProduct(plane2.getNormal());
            Vector3<float> n3CrossN1 = plane2.getNormal().crossProduct(plane0.getNormal());

            Point3 newPoint(n2CrossN3 * plane0.getDistanceToOrigin());
            newPoint += Point3(n3CrossN1 * plane1.getDistanceToOrigin());
            newPoint += Point3(n1CrossN2 * plane2.getDistanceToOrigin());
            newPoint *= -1.0f / plane0.getNormal().dotProduct(n2CrossN3);

            expandedPoints.emplace_back(newPoint);
        }

        return expandedPoints;
    }

    /**
     * Return expanded box surfaces. Surfaces are guaranteed to be in the following order: right, left, top, bottom, front, back
     * @param sortedOriginalPoints Original points in the following order: NTR, FTR, NBR, FBR, NTL, FTL, NBL, FBL
     * @param sortedExpandedPoints Expanded points in the following order: NTR, FTR, NBR, FBR, NTL, FTL, NBL, FBL
     */
    std::vector<std::shared_ptr<PolytopeSurface>> PolytopeBuilder::createExpandedPolytopeSurfaces(const std::vector<Point3<float>>& sortedOriginalPoints,
            const std::vector<Point3<float>>& sortedExpandedPoints, const NavMeshAgent& navMeshAgent) const {
        std::vector<std::shared_ptr<PolytopeSurface>> expandedSurfaces;
        expandedSurfaces.reserve(6);

        std::vector<Point3<float>> surfacePoints;
        surfacePoints.reserve(4);

        for (const auto& pointIndex : POINT_INDEX_TO_PLANES) {
            Vector3<float> v1 = sortedOriginalPoints[pointIndex[0]].vector(sortedOriginalPoints[pointIndex[2]]);
            Vector3<float> v2 = sortedOriginalPoints[pointIndex[1]].vector(sortedOriginalPoints[pointIndex[0]]);
            Vector3<float> normal = v1.crossProduct(v2).normalize();

            float angleToHorizontalInRadian = std::acos(normal.dotProduct(Vector3<float>(0.0, 1.0, 0.0)));
            bool isSlopeWalkable = std::fabs(angleToHorizontalInRadian) < navMeshAgent.getMaxSlope();

            surfacePoints.clear();

            if (isSlopeWalkable) { //walkable surfaces are not expanded on XZ axis to avoid character to walk outside the walkable surface

                Vector3<float> shiftVector = normal * navMeshAgent.computeExpandDistance(normal);
                for (std::size_t i = 0; i < 4; ++i) {
                    surfacePoints.push_back(sortedOriginalPoints[pointIndex[i]].translate(shiftVector));
                }

                std::vector<PlaneSurfaceSplit> planeSurfaceSplits = planeSurfaceSplitService.splitRectangleSurface(surfacePoints);
                for (const auto& planeSurfaceSplit : planeSurfaceSplits) {
                    expandedSurfaces.push_back(std::make_shared<PolytopePlaneSurface>(planeSurfaceSplit.planeSurfacePoints, normal, isSlopeWalkable));
                }
            } else {
                for (std::size_t i = 0; i < 4; ++i) {
                    surfacePoints.push_back(sortedExpandedPoints[pointIndex[i]]);
                }

                expandedSurfaces.push_back(std::make_shared<PolytopePlaneSurface>(surfacePoints, normal, isSlopeWalkable));
            }
        }

        return expandedSurfaces;
    }
}
