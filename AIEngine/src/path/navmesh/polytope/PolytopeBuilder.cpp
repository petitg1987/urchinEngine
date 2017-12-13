#include <cassert>

#include "PolytopeBuilder.h"
#include "path/navmesh/polytope/PolytopePlaneSurface.h"
#include "path/navmesh/polytope/PolytopeTerrainSurface.h"
#include "path/navmesh/polytope/services/TerrainObstacleService.h"

namespace urchin
{

    PointFace::PointFace(const Point3<float> &point, const std::vector<unsigned int> &faceIndices) :
        point(point),
        faceIndices(faceIndices)
    {

    }

    std::vector<std::unique_ptr<Polytope>> PolytopeBuilder::buildExpandedPolytopes(const std::shared_ptr<AIObject> &aiObject, const NavMeshAgent &agent)
    {
        std::vector<std::unique_ptr<Polytope>> expandedPolytopes;

        unsigned int aiShapeIndex = 0;
        for (auto &aiShape : aiObject->getShapes())
        {
            std::string shapeName = aiObject->getShapes().size()==1 ? aiObject->getName() : aiObject->getName() + "[" + std::to_string(aiShapeIndex++) + "]";
            Transform<float> shapeTransform = aiShape->hasLocalTransform() ? aiObject->getTransform() * aiShape->getLocalTransform() : aiObject->getTransform();
            std::unique_ptr<ConvexObject3D<float>> object = aiShape->getShape()->toConvexObject(shapeTransform);
            std::unique_ptr<Polytope> expandedPolytope;

            if (auto box = dynamic_cast<OBBox<float> *>(object.get()))
            {
                expandedPolytope = createExpandedPolytopeFor(shapeName, box, agent);
            } else if (auto capsule = dynamic_cast<Capsule<float> *>(object.get()))
            {
                expandedPolytope = createExpandedPolytopeFor(shapeName, capsule, agent);
            } else if (auto cone = dynamic_cast<Cone<float> *>(object.get()))
            {
                expandedPolytope = createExpandedPolytopeFor(shapeName, cone, agent);
            } else if (auto convexHull = dynamic_cast<ConvexHull3D<float> *>(object.get()))
            {
                expandedPolytope = createExpandedPolytopeFor(shapeName, convexHull, agent);
            } else if (auto cylinder = dynamic_cast<Cylinder<float> *>(object.get()))
            {
                expandedPolytope = createExpandedPolytopeFor(shapeName, cylinder, agent);
            } else if (auto sphere = dynamic_cast<Sphere<float> *>(object.get()))
            {
                expandedPolytope = createExpandedPolytopeFor(shapeName, sphere, agent);
            } else
            {
                throw std::invalid_argument("Shape type not supported by navigation mesh generator: " + std::string(typeid(*object).name()));
            }

            expandedPolytope->setObstacleCandidate(aiObject->isObstacleCandidate());
            expandedPolytopes.push_back(std::move(expandedPolytope));
        }

        return expandedPolytopes;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::buildExpandedPolytope(const std::shared_ptr<AITerrain> &aiTerrain, const std::shared_ptr<NavMeshConfig> &navMeshConfig)
    {
        #ifdef _DEBUG
            assert(MathAlgorithm::isOne(aiTerrain->getTransform().getScale()));
            assert(MathAlgorithm::isOne(aiTerrain->getTransform().getOrientationMatrix().determinant()));
        #endif

        std::vector<std::unique_ptr<PolytopeSurface>> expandedSurfaces;
        TerrainObstacleService terrainObstacleService(aiTerrain->getName(), aiTerrain->getTransform().getPosition(), aiTerrain->getVertices(),
                                                      aiTerrain->getXLength(), aiTerrain->getZLength());
        std::vector<CSGPolygon<float>> selfObstacles = terrainObstacleService.computeSelfObstacles(navMeshConfig->getMaxSlope());
        auto expandedSurface = std::make_unique<PolytopeTerrainSurface>(aiTerrain->getTransform().getPosition(), aiTerrain->getVertices(),
                                                                        aiTerrain->getXLength(), aiTerrain->getZLength(), selfObstacles);
        expandedSurface->setWalkableCandidate(true);
        expandedSurfaces.emplace_back(std::move(expandedSurface));

        auto expandedPolytope = std::make_unique<Polytope>(aiTerrain->getName(), expandedSurfaces);
        expandedPolytope->setWalkableCandidate(true);
        expandedPolytope->setObstacleCandidate(aiTerrain->isObstacleCandidate());
        return expandedPolytope;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytopeFor(const std::string &name, OBBox<float> *box, const NavMeshAgent &agent) const
    {
        std::vector<Point3<float>> expandedPoints = createExpandedPoints(box, agent);
        std::vector<std::unique_ptr<PolytopeSurface>> expandedPolytopeSurfaces = createExpandedPolytopeSurfaces(expandedPoints);
        return std::make_unique<Polytope>(name, expandedPolytopeSurfaces);
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytopeFor(const std::string &name, Capsule<float> *capsule, const NavMeshAgent &agent) const
    {
        Vector3<float> boxHalfSizes(capsule->getRadius(), capsule->getRadius(), capsule->getRadius());
        boxHalfSizes[capsule->getCapsuleOrientation()] += capsule->getCylinderHeight() / 2.0f;

        OBBox<float> capsuleBox(boxHalfSizes, capsule->getCenterOfMass(), capsule->getOrientation());
        std::unique_ptr<Polytope> polytope = createExpandedPolytopeFor(name, &capsuleBox, agent);
        polytope->setWalkableCandidate(false);

        return polytope;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytopeFor(const std::string &name, Cone<float> *cone, const NavMeshAgent &agent) const
    {
        Vector3<float> boxHalfSizes(cone->getRadius(), cone->getRadius(), cone->getRadius());
        boxHalfSizes[cone->getConeOrientation()/2] = cone->getHeight() / 2.0f;

        OBBox<float> coneBox(boxHalfSizes, cone->getCenter(), cone->getOrientation());
        std::unique_ptr<Polytope> polytope = createExpandedPolytopeFor(name, &coneBox, agent);
        polytope->setWalkableCandidate(false);

        return polytope;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytopeFor(const std::string &name, ConvexHull3D<float> *convexHull, const NavMeshAgent &agent) const
    {
        const std::map<unsigned int, IndexedTriangle3D<float>> &indexedTriangles = convexHull->getIndexedTriangles();
        const std::map<unsigned int, ConvexHullPoint<float>> &convexHullPoints = convexHull->getConvexHullPoints();

        std::vector<PointFace> pointFaces;
        pointFaces.reserve(convexHullPoints.size());
        for(const auto &convexHullPoint : convexHullPoints)
        {
            pointFaces.emplace_back(PointFace(convexHullPoint.second.point, {}));
        }

        std::vector<std::vector<unsigned int>> pointIndicesByFaces;
        pointIndicesByFaces.reserve(indexedTriangles.size());
        std::vector<Plane<float>> expandedPlanes;
        expandedPlanes.reserve(indexedTriangles.size());
        unsigned int faceIndex=0;
        for(const auto &indexedTriangle : indexedTriangles)
        {
            const unsigned int *indices = indexedTriangle.second.getIndices();

            unsigned int point0Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[0])));
            pointFaces[point0Index].faceIndices.push_back(faceIndex);

            unsigned int point1Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[1])));
            pointFaces[point1Index].faceIndices.push_back(faceIndex);

            unsigned int point2Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[2])));
            pointFaces[point2Index].faceIndices.push_back(faceIndex);

            pointIndicesByFaces.emplace_back(std::vector<unsigned int>({point0Index, point1Index, point2Index}));
            expandedPlanes.emplace_back(createExpandedPlane(pointFaces[point0Index].point, pointFaces[point1Index].point, pointFaces[point2Index].point, agent));

            faceIndex++;
        }

        std::vector<Point3<float>> expandedPoints = expandPoints(pointFaces, expandedPlanes);
        std::vector<std::unique_ptr<PolytopeSurface>> expandedSurfaces;
        expandedSurfaces.reserve(pointIndicesByFaces.size());
        for(const auto &pointIndicesByFace : pointIndicesByFaces)
        {
            expandedSurfaces.emplace_back(std::make_unique<PolytopePlaneSurface>(pointIndicesByFace, expandedPoints));
        }

        return std::make_unique<Polytope>(name, expandedSurfaces);
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytopeFor(const std::string &name, Cylinder<float> *cylinder, const NavMeshAgent &agent) const
    {
        Vector3<float> boxHalfSizes(cylinder->getRadius(), cylinder->getRadius(), cylinder->getRadius());
        boxHalfSizes[cylinder->getCylinderOrientation()] = cylinder->getHeight() / 2.0f;

        OBBox<float> cylinderBox(boxHalfSizes, cylinder->getCenterOfMass(), cylinder->getOrientation());
        std::vector<Point3<float>> expandedPoints = createExpandedPoints(&cylinderBox, agent);

        std::vector<std::unique_ptr<PolytopeSurface>> expandedSurfaces = createExpandedPolytopeSurfaces(expandedPoints);
        for(unsigned int i=0; i<expandedSurfaces.size(); ++i)
        {
            expandedSurfaces[i]->setWalkableCandidate(cylinder->getCylinderOrientation()==i/2);
        }

        return std::make_unique<Polytope>(name, expandedSurfaces);
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createExpandedPolytopeFor(const std::string &name, Sphere<float> *sphere, const NavMeshAgent &agent) const
    {
        OBBox<float> sphereBox(*sphere);
        std::unique_ptr<Polytope> polytope = createExpandedPolytopeFor(name, &sphereBox, agent);
        polytope->setWalkableCandidate(false);

        return polytope;
    }

    /**
     * Return box points. This method suppose that faces index (0 to 5) are in the following order: right, left, top, bottom, front, back.
     */
    std::vector<Point3<float>> PolytopeBuilder::createExpandedPoints(OBBox<float> *box, const NavMeshAgent &navMeshAgent) const
    {
        std::vector<PointFace> pointFaces;
        pointFaces.reserve(8);

        std::vector<Point3<float>> points = box->getPoints();
        constexpr unsigned int faceIndicesTab[8][3] = {{0, 2, 4}, {0, 2, 5}, {0, 3, 4}, {0, 3, 5}, {1, 2, 4}, {1, 2, 5}, {1, 3, 4}, {1, 3, 5}};
        for(unsigned int i=0; i<8; ++i)
        {
            pointFaces.emplace_back(PointFace(points[i], {faceIndicesTab[i][0], faceIndicesTab[i][1], faceIndicesTab[i][2]}));
        }

        std::vector<Plane<float>> expandedPlanes = createExpandedBoxPlanes(points, navMeshAgent);
        return expandPoints(pointFaces, expandedPlanes);
    }

    /**
     * Returns expanded planes. Planes are guaranteed to be in the following order: right, left, top, bottom, front, back
     * @param points Points are sorted first on positive X axis, then on positive Y axis and then on positive Z axis
     */
    std::vector<Plane<float>> PolytopeBuilder::createExpandedBoxPlanes(const std::vector<Point3<float>> &points, const NavMeshAgent &navMeshAgent) const
    {
        std::vector<Plane<float>> expandedPlanes;
        expandedPlanes.reserve(6);

        expandedPlanes.emplace_back(createExpandedPlane(points[0], points[2], points[3], navMeshAgent));
        expandedPlanes.emplace_back(createExpandedPlane(points[4], points[5], points[7], navMeshAgent));
        expandedPlanes.emplace_back(createExpandedPlane(points[0], points[1], points[5], navMeshAgent));
        expandedPlanes.emplace_back(createExpandedPlane(points[3], points[2], points[6], navMeshAgent));
        expandedPlanes.emplace_back(createExpandedPlane(points[0], points[4], points[6], navMeshAgent));
        expandedPlanes.emplace_back(createExpandedPlane(points[1], points[3], points[7], navMeshAgent));

        return expandedPlanes;
    }

    Plane<float> PolytopeBuilder::createExpandedPlane(const Point3<float> &p1, const Point3<float> &p2, const Point3<float> &p3, const NavMeshAgent &navMeshAgent) const
    {
        Plane<float> plane(p1, p2, p3);

        float expandDistance = navMeshAgent.computeExpandDistance(plane.getNormal());
        plane.setDistanceToOrigin(plane.getDistanceToOrigin() - expandDistance);

        return plane;
    }

    std::vector<Point3<float>> PolytopeBuilder::expandPoints(const std::vector<PointFace> &pointFaces, const std::vector<Plane<float>> &expandedPlanes) const
    {
        std::vector<Point3<float>> expandedPoints;
        expandedPoints.reserve(pointFaces.size());

        for(auto &pointFace : pointFaces)
        {
            #ifdef _DEBUG
                if(pointFace.faceIndices.size() < 3)
                {
                    throw std::runtime_error("A polyhedron (3D) point must belong at least to 3 faces");
                }
            #endif

            std::vector<Plane<float>> threePlanes = findThreeNonParallelPlanes(pointFace.faceIndices, expandedPlanes);
            Point3<float> newPoint;
            if (threePlanes.size() == 3)
            {
                Vector3<float> n1CrossN2 = threePlanes[0].getNormal().crossProduct(threePlanes[1].getNormal());
                Vector3<float> n2CrossN3 = threePlanes[1].getNormal().crossProduct(threePlanes[2].getNormal());
                Vector3<float> n3CrossN1 = threePlanes[2].getNormal().crossProduct(threePlanes[0].getNormal());

                newPoint = Point3<float>(n2CrossN3 * threePlanes[0].getDistanceToOrigin());
                newPoint += Point3<float>(n3CrossN1 * threePlanes[1].getDistanceToOrigin());
                newPoint += Point3<float>(n1CrossN2 * threePlanes[2].getDistanceToOrigin());
                newPoint *= -1.0 / threePlanes[0].getNormal().dotProduct(n2CrossN3);
            } else
            { //useless point found on polytope (could be removed from polytope without impact)
                std::stringstream logStream;
                logStream.precision(std::numeric_limits<float>::max_digits10);
                logStream << "Impossible to resize polytope because of useless point." << std::endl;
                logStream << " - Point: " << pointFace.point << " (";
                for(unsigned int faceIndex : pointFace.faceIndices)
                {
                    logStream << faceIndex <<" ";
                }
                logStream << ")"<<std::endl;
                logStream << " - Expanded planes: " << std::endl;
                for(const auto &expandedPlane : expandedPlanes)
                {
                    logStream << expandedPlane <<std::endl;
                }
                Logger::logger().logError(logStream.str());
            }

            expandedPoints.emplace_back(newPoint);
        }

        return expandedPoints;
    }

    std::vector<Plane<float>> PolytopeBuilder::findThreeNonParallelPlanes(const std::vector<unsigned int> &planeIndices, const std::vector<Plane<float>> &allPlanes) const
    {
        constexpr float PARALLEL_COMPARISON_TOLERANCE = 0.01f;

        std::vector<Plane<float>> nonParallelPlanes;
        nonParallelPlanes.reserve(3);

        Plane<float> plane1 = allPlanes.at(planeIndices[0]);
        for(unsigned int i=1; i<planeIndices.size(); ++i)
        {
            Plane<float> plane2 = allPlanes.at(planeIndices[i]);
            if(plane1.getNormal().crossProduct(plane2.getNormal()).squareLength() < PARALLEL_COMPARISON_TOLERANCE)
            { //planes are parallel: continue on next plane
                continue;
            }

            for(unsigned int j=i+1; j<planeIndices.size(); ++j)
            {
                Plane<float> plane3 = allPlanes.at(planeIndices[j]);

                Vector3<float> n2CrossN3 = plane2.getNormal().crossProduct(plane3.getNormal());
                if(n2CrossN3.squareLength() < 0.0
                   || plane3.getNormal().crossProduct(plane1.getNormal()).squareLength() < PARALLEL_COMPARISON_TOLERANCE
                   || plane1.getNormal().dotProduct(n2CrossN3)==0.0) //additional check due to float imprecision
                { //planes are parallel: continue on next plane
                    continue;
                }

                nonParallelPlanes = {plane1, plane2, plane3};
                return nonParallelPlanes;
            }
        }

        return nonParallelPlanes;
    }

    /**
     * Return expanded box surfaces. Surfaces are guaranteed to be in the following order: right, left, top, bottom, front, back when
     * points are sorted first on positive X axis, then on positive Y axis and then on positive Z axis.
     */
    std::vector<std::unique_ptr<PolytopeSurface>> PolytopeBuilder::createExpandedPolytopeSurfaces(const std::vector<Point3<float>> &expandedPoints) const
    {
        std::vector<std::unique_ptr<PolytopeSurface>> expandedSurfaces;
        expandedSurfaces.reserve(6);

        expandedSurfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({0, 2, 3, 1}), expandedPoints)); //right
        expandedSurfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({4, 5, 7, 6}), expandedPoints)); //left
        expandedSurfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({0, 1, 5, 4}), expandedPoints)); //top
        expandedSurfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({3, 2, 6, 7}), expandedPoints)); //bottom
        expandedSurfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({0, 4, 6, 2}), expandedPoints)); //front
        expandedSurfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({1, 3, 7, 5}), expandedPoints)); //back

        return expandedSurfaces;
    }
}
