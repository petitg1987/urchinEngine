#include <debug/NavMeshDisplayer.h>

namespace urchin {

    NavMeshDisplayer::NavMeshDisplayer(AIEnvironment& aiEnvironment, Renderer3d& renderer3d) :
            aiEnvironment(aiEnvironment),
            renderer3d(renderer3d),
            loadedNavMeshId(std::numeric_limits<unsigned int>::max()) {
            
    }
    
    NavMeshDisplayer::~NavMeshDisplayer() {
        clearDisplay();
    }

    void NavMeshDisplayer::display() {
        NavMesh navMesh = aiEnvironment.getNavMeshGenerator().copyLastGeneratedNavMesh();

        if (loadedNavMeshId != navMesh.getUpdateId()) {
            clearDisplay();

            std::vector<Point3<float>> triangleMeshPoints;
            std::vector<Point3<float>> triangleJumpPoints;

            for (const auto& navPolygon : navMesh.getPolygons()) {
                for (const auto& triangle : navPolygon->getTriangles()) {
                    triangleMeshPoints.emplace_back(navPolygon->getPoints()[triangle->getIndex(0)]);
                    triangleMeshPoints.emplace_back(navPolygon->getPoints()[triangle->getIndex(1)]);
                    triangleMeshPoints.emplace_back(navPolygon->getPoints()[triangle->getIndex(2)]);

                    for (const auto& link : triangle->getLinks()) {
                        if (link->getLinkType() == JUMP) {
                            LineSegment3D<float> endEdge = link->getTargetTriangle()->computeEdge(link->getLinkConstraint()->getTargetEdgeIndex());
                            LineSegment3D<float> constrainedStartEdge = link->getLinkConstraint()->computeSourceJumpEdge(triangle->computeEdge(link->getSourceEdgeIndex()));
                            LineSegment3D constrainedEndEdge(endEdge.closestPoint(constrainedStartEdge.getA()), endEdge.closestPoint(constrainedStartEdge.getB()));

                            triangleJumpPoints.emplace_back(constrainedStartEdge.getA());
                            triangleJumpPoints.emplace_back(constrainedStartEdge.getB());
                            triangleJumpPoints.emplace_back(constrainedEndEdge.getA());

                            triangleJumpPoints.emplace_back(constrainedStartEdge.getA());
                            triangleJumpPoints.emplace_back(constrainedEndEdge.getA());
                            triangleJumpPoints.emplace_back(constrainedEndEdge.getB());
                        }
                    }
                }
            }

            if (!triangleMeshPoints.empty()) {
                auto meshModel = std::make_shared<TrianglesModel>(toDisplayPoints(triangleMeshPoints, 0.02f, 0.98f));
                addNavMeshModel(meshModel, Vector3<float>(0.0, 0.0, 1.0));
            }

            if (!triangleJumpPoints.empty()) {
                auto jumpModel = std::make_shared<TrianglesModel>(triangleJumpPoints);
                addNavMeshModel(jumpModel, Vector3<float>(0.5, 0.0, 0.5));
            }

            loadedNavMeshId = navMesh.getUpdateId();
        }
    }

    void NavMeshDisplayer::clearDisplay() {
        for (const auto& navMeshModel : navMeshModels) {
            renderer3d.getGeometryContainer().removeGeometry(*navMeshModel);
        }
        navMeshModels.clear();
    }

    std::vector<Point3<float>> NavMeshDisplayer::toDisplayPoints(const std::vector<Point3<float>>& points, float yElevation, float shrinkFactor) const {
        std::vector<Point3<float>> displayPoints;
        displayPoints.reserve(points.size());

        //elevation
        for (const auto& point : points) {
            displayPoints.emplace_back(point.X, point.Y + yElevation, point.Z);
        }

        //shrink triangles
        for (std::size_t i = 0; i < displayPoints.size(); i += 3) {
            Point3<float> a = displayPoints[i];
            Point3<float> b = displayPoints[i + 1];
            Point3<float> c = displayPoints[i + 2];
            Point3<float> centroid = (a + b + c) / 3.0f;

            displayPoints[i] = centroid + shrinkFactor * (a - centroid);
            displayPoints[i + 1] = centroid + shrinkFactor * (b - centroid);
            displayPoints[i + 2] = centroid + shrinkFactor * (c - centroid);
        }

        return displayPoints;
    }

    void NavMeshDisplayer::addNavMeshModel(std::shared_ptr<GeometryModel> model, const Vector3<float>& color) {
        model->setColor(color.X, color.Y, color.Z);
        model->disableCullFace();
        navMeshModels.push_back(model);
        renderer3d.getGeometryContainer().addGeometry(std::move(model));
    }
}
