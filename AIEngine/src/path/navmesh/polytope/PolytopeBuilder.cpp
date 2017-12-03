#include <cassert>

#include "PolytopeBuilder.h"
#include "path/navmesh/polytope/PolytopePlaneSurface.h"
#include "path/navmesh/polytope/PolytopeTerrainSurface.h"

namespace urchin
{

    std::vector<std::unique_ptr<Polytope>> PolytopeBuilder::buildPolytopes(const std::shared_ptr<AIObject> &aiObject)
    {
        std::vector<std::unique_ptr<Polytope>> polytopes;

        unsigned int aiShapeIndex = 0;
        for (auto &aiShape : aiObject->getShapes())
        {
            std::string shapeName = aiObject->getShapes().size()==1 ? aiObject->getName() : aiObject->getName() + "[" + std::to_string(aiShapeIndex++) + "]";
            Transform<float> shapeTransform = aiShape->hasLocalTransform() ? aiObject->getTransform() * aiShape->getLocalTransform() : aiObject->getTransform();
            std::unique_ptr<ConvexObject3D<float>> object = aiShape->getShape()->toConvexObject(shapeTransform);
            std::unique_ptr<Polytope> polytope;

            if (auto box = dynamic_cast<OBBox<float> *>(object.get()))
            {
                polytope = createPolytopeFor(shapeName, box);
            } else if (auto capsule = dynamic_cast<Capsule<float> *>(object.get()))
            {
                polytope = createPolytopeFor(shapeName, capsule);
            } else if (auto cone = dynamic_cast<Cone<float> *>(object.get()))
            {
                polytope = createPolytopeFor(shapeName, cone);
            } else if (auto convexHull = dynamic_cast<ConvexHull3D<float> *>(object.get()))
            {
                polytope = createPolytopeFor(shapeName, convexHull);
            } else if (auto cylinder = dynamic_cast<Cylinder<float> *>(object.get()))
            {
                polytope = createPolytopeFor(shapeName, cylinder);
            } else if (auto sphere = dynamic_cast<Sphere<float> *>(object.get()))
            {
                polytope = createPolytopeFor(shapeName, sphere);
            } else
            {
                throw std::invalid_argument("Shape type not supported by navigation mesh generator: " + std::string(typeid(*object).name()));
            }

            polytope->setObstacleCandidate(aiObject->isObstacleCandidate());
            polytopes.push_back(std::move(polytope));
        }

        return polytopes;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::buildPolytope(const std::shared_ptr<AITerrain> &aiTerrain)
    {
        //TODO build polytope from terrain

        std::vector<std::unique_ptr<PolytopeSurface>> surfaces;
        surfaces.emplace_back(std::make_unique<PolytopeTerrainSurface>());

        std::vector<PolytopePoint> points;

        return std::make_unique<Polytope>(aiTerrain->getName(), surfaces, points);
    }

    /**
     * Return box surfaces. Surfaces are guaranteed to be in the following order: right, left, top, bottom, front, back when
     * points are sorted first on positive X axis, then on positive Y axis and then on positive Z axis.
     */
    std::vector<std::unique_ptr<PolytopeSurface>> PolytopeBuilder::createPolytopeSurfaces(const std::vector<PolytopePoint> &points) const
    {
        std::vector<std::unique_ptr<PolytopeSurface>> surfaces;
        surfaces.reserve(6);

        surfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({0, 2, 3, 1}), points)); //right
        surfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({4, 5, 7, 6}), points)); //left
        surfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({0, 1, 5, 4}), points)); //top
        surfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({3, 2, 6, 7}), points)); //bottom
        surfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({0, 4, 6, 2}), points)); //front
        surfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({1, 3, 7, 5}), points)); //back

        return surfaces;
    }

    /**
     * Return box points. This method suppose that faces index (0 to 5) are in the following order: right, left, top, bottom, front, back.
     */
    std::vector<PolytopePoint> PolytopeBuilder::createPolytopePoints(OBBox<float> *box) const
    {
        std::vector<PolytopePoint> polytopePoints;
        polytopePoints.reserve(8);

        std::vector<Point3<float>> points = box->getPoints();
        constexpr unsigned int faceIndicesTab[8][3] = {{0, 2, 4}, {0, 2, 5}, {0, 3, 4}, {0, 3, 5}, {1, 2, 4}, {1, 2, 5}, {1, 3, 4}, {1, 3, 5}};
        for(unsigned int i=0; i<8; ++i)
        {
            PolytopePoint polytopePoint(points[i]);
            polytopePoint.addFaceIndex(faceIndicesTab[i][0]);
            polytopePoint.addFaceIndex(faceIndicesTab[i][1]);
            polytopePoint.addFaceIndex(faceIndicesTab[i][2]);

            polytopePoints.push_back(polytopePoint);
        }

        return polytopePoints;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createPolytopeFor(const std::string &name, OBBox<float> *box) const
    {
        std::vector<PolytopePoint> polytopePoint = createPolytopePoints(box);
        std::vector<std::unique_ptr<PolytopeSurface>> polytopeSurfaces = createPolytopeSurfaces(polytopePoint);
        return std::make_unique<Polytope>(name, polytopeSurfaces, polytopePoint);
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createPolytopeFor(const std::string &name, Capsule<float> *capsule) const
    {
        Vector3<float> boxHalfSizes(capsule->getRadius(), capsule->getRadius(), capsule->getRadius());
        boxHalfSizes[capsule->getCapsuleOrientation()] += capsule->getCylinderHeight() / 2.0f;

        OBBox<float> capsuleBox(boxHalfSizes, capsule->getCenterOfMass(), capsule->getOrientation());
        std::unique_ptr<Polytope> polytope = createPolytopeFor(name, &capsuleBox);
        polytope->setWalkableCandidate(false);

        return polytope;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createPolytopeFor(const std::string &name, Cone<float> *cone) const
    {
        Vector3<float> boxHalfSizes(cone->getRadius(), cone->getRadius(), cone->getRadius());
        boxHalfSizes[cone->getConeOrientation()/2] = cone->getHeight() / 2.0f;

        OBBox<float> coneBox(boxHalfSizes, cone->getCenter(), cone->getOrientation());
        std::unique_ptr<Polytope> polytope = createPolytopeFor(name, &coneBox);
        polytope->setWalkableCandidate(false);

        return polytope;
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createPolytopeFor(const std::string &name, ConvexHull3D<float> *convexHull) const
    {
        const std::map<unsigned int, IndexedTriangle3D<float>> &indexedTriangles = convexHull->getIndexedTriangles();
        const std::map<unsigned int, ConvexHullPoint<float>> &convexHullPoints = convexHull->getConvexHullPoints();

        std::vector<PolytopePoint> polytopePoints;
        polytopePoints.reserve(convexHullPoints.size());
        for(const auto &convexHullPoint : convexHullPoints)
        {
            polytopePoints.emplace_back(PolytopePoint(convexHullPoint.second.point));
        }

        std::vector<std::unique_ptr<PolytopeSurface>> surfaces;
        surfaces.reserve(indexedTriangles.size());
        unsigned int i=0;
        for(const auto &indexedTriangle : indexedTriangles)
        {
            const unsigned int *indices = indexedTriangle.second.getIndices();

            unsigned int polytopePoint0Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[0])));
            polytopePoints[polytopePoint0Index].addFaceIndex(i);

            unsigned int polytopePoint1Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[1])));
            polytopePoints[polytopePoint1Index].addFaceIndex(i);

            unsigned int polytopePoint2Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[2])));
            polytopePoints[polytopePoint2Index].addFaceIndex(i);

            surfaces.push_back(std::make_unique<PolytopePlaneSurface>(std::vector<unsigned int>({polytopePoint0Index, polytopePoint1Index, polytopePoint2Index}), polytopePoints));
            i++;
        }

        return std::make_unique<Polytope>(name, surfaces, polytopePoints);
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createPolytopeFor(const std::string &name, Cylinder<float> *cylinder) const
    {
        Vector3<float> boxHalfSizes(cylinder->getRadius(), cylinder->getRadius(), cylinder->getRadius());
        boxHalfSizes[cylinder->getCylinderOrientation()] = cylinder->getHeight() / 2.0f;

        OBBox<float> cylinderBox(boxHalfSizes, cylinder->getCenterOfMass(), cylinder->getOrientation());
        std::vector<PolytopePoint> polytopePoint = createPolytopePoints(&cylinderBox);

        std::vector<std::unique_ptr<PolytopeSurface>> surfaces = createPolytopeSurfaces(polytopePoint);
        for(unsigned int i=0; i<surfaces.size(); ++i)
        {
            surfaces[i]->setWalkableCandidate(cylinder->getCylinderOrientation()==i/2); //TODO is it correct ?
        }

        return std::make_unique<Polytope>(name, surfaces, polytopePoint);
    }

    std::unique_ptr<Polytope> PolytopeBuilder::createPolytopeFor(const std::string &name, Sphere<float> *sphere) const
    {
        OBBox<float> sphereBox(*sphere);
        std::unique_ptr<Polytope> polytope = createPolytopeFor(name, &sphereBox);
        polytope->setWalkableCandidate(false);

        return polytope;
    }
}
