#include <cassert>

#include "PolyhedronBuilder.h"

namespace urchin
{

    std::vector<std::unique_ptr<Polyhedron>> PolyhedronBuilder::buildPolyhedrons(const std::shared_ptr<AIObject> &aiObject)
    {
        std::vector<std::unique_ptr<Polyhedron>> polyhedrons;

        unsigned int aiShapeIndex = 0;
        for (auto &aiShape : aiObject->getShapes())
        {
            std::string shapeName = aiObject->getShapes().size()==1 ? aiObject->getName() : aiObject->getName() + "[" + std::to_string(aiShapeIndex++) + "]";
            Transform<float> shapeTransform = aiShape->hasLocalTransform() ? aiObject->getTransform() * aiShape->getLocalTransform() : aiObject->getTransform();
            std::unique_ptr<ConvexObject3D<float>> object = aiShape->getShape()->toConvexObject(shapeTransform);
            std::unique_ptr<Polyhedron> polyhedron;

            if (auto box = dynamic_cast<OBBox<float> *>(object.get()))
            {
                polyhedron = createPolyhedronFor(shapeName, box);
            } else if (auto capsule = dynamic_cast<Capsule<float> *>(object.get()))
            {
                polyhedron = createPolyhedronFor(shapeName, capsule);
            } else if (auto cone = dynamic_cast<Cone<float> *>(object.get()))
            {
                polyhedron = createPolyhedronFor(shapeName, cone);
            } else if (auto convexHull = dynamic_cast<ConvexHull3D<float> *>(object.get()))
            {
                polyhedron = createPolyhedronFor(shapeName, convexHull);
            } else if (auto cylinder = dynamic_cast<Cylinder<float> *>(object.get()))
            {
                polyhedron = createPolyhedronFor(shapeName, cylinder);
            } else if (auto sphere = dynamic_cast<Sphere<float> *>(object.get()))
            {
                polyhedron = createPolyhedronFor(shapeName, sphere);
            } else
            {
                throw std::invalid_argument("Shape type not supported by navigation mesh generator: " + std::string(typeid(*object).name()));
            }

            polyhedron->setObstacleCandidate(aiObject->isObstacleCandidate());
            polyhedrons.push_back(std::move(polyhedron));
        }

        return polyhedrons;
    }

    /**
     * Return box faces. Faces are guaranteed to be in the following order: right, left, top, bottom, front, back when
     * points are sorted first on positive X axis, then on positive Y axis and then on positive Z axis.
     */
    std::vector<PolyhedronFace> PolyhedronBuilder::createPolyhedronFaces() const
    {
        std::vector<PolyhedronFace> faces;
        faces.reserve(6);

        faces.push_back(PolyhedronFace({0, 2, 3, 1})); //right
        faces.push_back(PolyhedronFace({4, 5, 7, 6})); //left
        faces.push_back(PolyhedronFace({0, 1, 5, 4})); //top
        faces.push_back(PolyhedronFace({3, 2, 6, 7})); //bottom
        faces.push_back(PolyhedronFace({0, 4, 6, 2})); //front
        faces.push_back(PolyhedronFace({1, 3, 7, 5})); //back

        return faces;
    }

    /**
     * Return box points. This method suppose that faces index (0 to 5) are in the following order: right, left, top, bottom, front, back.
     */
    std::vector<PolyhedronPoint> PolyhedronBuilder::createPolyhedronPoints(OBBox<float> *box) const
    {
        std::vector<PolyhedronPoint> polyhedronPoints;
        polyhedronPoints.reserve(8);

        std::vector<Point3<float>> points = box->getPoints();
        constexpr unsigned int faceIndicesTab[8][3] = {{0, 2, 4}, {0, 2, 5}, {0, 3, 4}, {0, 3, 5}, {1, 2, 4}, {1, 2, 5}, {1, 3, 4}, {1, 3, 5}};
        for(unsigned int i=0; i<8; ++i)
        {
            PolyhedronPoint polyhedronPoint(points[i]);
            polyhedronPoint.addFaceIndex(faceIndicesTab[i][0]);
            polyhedronPoint.addFaceIndex(faceIndicesTab[i][1]);
            polyhedronPoint.addFaceIndex(faceIndicesTab[i][2]);

            polyhedronPoints.push_back(polyhedronPoint);
        }

        return polyhedronPoints;
    }

    std::unique_ptr<Polyhedron> PolyhedronBuilder::createPolyhedronFor(const std::string &name, OBBox<float> *box) const
    {
        std::vector<PolyhedronFace> polyhedronFaces = createPolyhedronFaces();
        return std::make_unique<Polyhedron>(name, polyhedronFaces, createPolyhedronPoints(box));
    }

    std::unique_ptr<Polyhedron> PolyhedronBuilder::createPolyhedronFor(const std::string &name, Capsule<float> *capsule) const
    {
        Vector3<float> boxHalfSizes(capsule->getRadius(), capsule->getRadius(), capsule->getRadius());
        boxHalfSizes[capsule->getCapsuleOrientation()] += capsule->getCylinderHeight() / 2.0f;

        OBBox<float> capsuleBox(boxHalfSizes, capsule->getCenterOfMass(), capsule->getOrientation());
        std::unique_ptr<Polyhedron> polyhedron = createPolyhedronFor(name, &capsuleBox);
        polyhedron->setWalkableCandidate(false);

        return polyhedron;
    }

    std::unique_ptr<Polyhedron> PolyhedronBuilder::createPolyhedronFor(const std::string &name, Cone<float> *cone) const
    {
        Vector3<float> boxHalfSizes(cone->getRadius(), cone->getRadius(), cone->getRadius());
        boxHalfSizes[cone->getConeOrientation()/2] = cone->getHeight() / 2.0f;

        OBBox<float> coneBox(boxHalfSizes, cone->getCenter(), cone->getOrientation());
        std::unique_ptr<Polyhedron> polyhedron = createPolyhedronFor(name, &coneBox);
        polyhedron->setWalkableCandidate(false);

        return polyhedron;
    }

    std::unique_ptr<Polyhedron> PolyhedronBuilder::createPolyhedronFor(const std::string &name, ConvexHull3D<float> *convexHull) const
    {
        const std::map<unsigned int, IndexedTriangle3D<float>> &indexedTriangles = convexHull->getIndexedTriangles();
        const std::map<unsigned int, ConvexHullPoint<float>> &convexHullPoints = convexHull->getConvexHullPoints();

        std::vector<PolyhedronPoint> polyhedronPoints;
        polyhedronPoints.reserve(convexHullPoints.size());
        for(const auto &convexHullPoint : convexHullPoints)
        {
            polyhedronPoints.emplace_back(PolyhedronPoint(convexHullPoint.second.point));
        }

        std::vector<PolyhedronFace> faces;
        faces.reserve(indexedTriangles.size());
        unsigned int i=0;
        for(const auto &indexedTriangle : indexedTriangles)
        {
            const unsigned int *indices = indexedTriangle.second.getIndices();

            unsigned int polyhedronPoint0Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[0])));
            polyhedronPoints[polyhedronPoint0Index].addFaceIndex(i);

            unsigned int polyhedronPoint1Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[1])));
            polyhedronPoints[polyhedronPoint1Index].addFaceIndex(i);

            unsigned int polyhedronPoint2Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[2])));
            polyhedronPoints[polyhedronPoint2Index].addFaceIndex(i);

            faces.push_back(PolyhedronFace({polyhedronPoint0Index, polyhedronPoint1Index, polyhedronPoint2Index}));
            i++;
        }

        return std::make_unique<Polyhedron>(name, faces, polyhedronPoints);
    }

    std::unique_ptr<Polyhedron> PolyhedronBuilder::createPolyhedronFor(const std::string &name, Cylinder<float> *cylinder) const
    {
        Vector3<float> boxHalfSizes(cylinder->getRadius(), cylinder->getRadius(), cylinder->getRadius());
        boxHalfSizes[cylinder->getCylinderOrientation()] = cylinder->getHeight() / 2.0f;

        OBBox<float> cylinderBox(boxHalfSizes, cylinder->getCenterOfMass(), cylinder->getOrientation());
        std::vector<PolyhedronFace> faces = createPolyhedronFaces();
        for(unsigned int i=0; i<faces.size(); ++i)
        {
            faces[i].setWalkableCandidate(cylinder->getCylinderOrientation()==i/2);
        }

        return std::make_unique<Polyhedron>(name, faces, createPolyhedronPoints(&cylinderBox));
    }

    std::unique_ptr<Polyhedron> PolyhedronBuilder::createPolyhedronFor(const std::string &name, Sphere<float> *sphere) const
    {
        OBBox<float> sphereBox(*sphere);
        std::unique_ptr<Polyhedron> polyhedron = createPolyhedronFor(name, &sphereBox);
        polyhedron->setWalkableCandidate(false);

        return polyhedron;
    }
}
