#include "NavMeshDisplayer.h"

namespace urchin {

    NavMeshDisplayer::NavMeshDisplayer(AIManager* aiManager, Renderer3d* renderer3d) :
        aiManager(aiManager),
        renderer3d(renderer3d),
        loadedNavMeshId(std::numeric_limits<unsigned int>::max()) {
            
    }
    
    NavMeshDisplayer::~NavMeshDisplayer() {
        clearDisplay();
    }

    void NavMeshDisplayer::display() {
        NavMesh navMesh = aiManager->getNavMeshGenerator()->copyLastGeneratedNavMesh();

        if (loadedNavMeshId != navMesh.getUpdateId()) {
            clearDisplay();

            std::vector<Point3<float>> triangleMeshPoints;
            std::vector<Point3<float>> triangleJumpPoints;

            for (const auto &navPolygon : navMesh.getPolygons()) {
                for (const auto &triangle : navPolygon->getTriangles()) {
                    triangleMeshPoints.emplace_back(navPolygon->getPoints()[triangle->getIndex(0)]);
                    triangleMeshPoints.emplace_back(navPolygon->getPoints()[triangle->getIndex(1)]);
                    triangleMeshPoints.emplace_back(navPolygon->getPoints()[triangle->getIndex(2)]);

                    for (const auto &link : triangle->getLinks()) {
                        if (link->getLinkType() == NavLinkType::JUMP) {
                            LineSegment3D<float> endEdge = link->getTargetTriangle()->computeEdge(link->getLinkConstraint()->getTargetEdgeIndex());
                            LineSegment3D<float> constrainedStartEdge = link->getLinkConstraint()->computeSourceJumpEdge(triangle->computeEdge(link->getSourceEdgeIndex()));
                            LineSegment3D<float> constrainedEndEdge(endEdge.closestPoint(constrainedStartEdge.getA()), endEdge.closestPoint(constrainedStartEdge.getB()));

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

            if(!triangleMeshPoints.empty()) {
                auto *meshModel = new TrianglesModel(toDisplayPoints(triangleMeshPoints, 0.02f));
                addNavMeshModel(meshModel, PolygonMode::FILL, Vector3<float>(0.0, 0.0, 1.0));

                auto *meshWireframeModel = new TrianglesModel(toDisplayPoints(triangleMeshPoints, 0.025f));
                addNavMeshModel(meshWireframeModel, PolygonMode::WIREFRAME, Vector3<float>(0.5, 0.5, 1.0));
            }

            if(!triangleJumpPoints.empty()) {
                auto *jumpModel = new TrianglesModel(triangleJumpPoints);
                addNavMeshModel(jumpModel, PolygonMode::FILL, Vector3<float>(0.5, 0.0, 0.5));
            }

            loadedNavMeshId = navMesh.getUpdateId();
        }
    }

    void NavMeshDisplayer::clearDisplay() {
        for (auto navMeshModel : navMeshModels) {
            renderer3d->getGeometryManager()->removeGeometry(navMeshModel);
            delete navMeshModel;
        }
        navMeshModels.clear();
    }

    std::vector<Point3<float>> NavMeshDisplayer::toDisplayPoints(const std::vector<Point3<float>> &points, float yElevation) const {
        std::vector<Point3<float>> displayPoints;
        displayPoints.reserve(points.size());

        for (const auto &point : points) {
            displayPoints.emplace_back(Point3<float>(point.X, point.Y + yElevation, point.Z));
        }

        return displayPoints;
    }

    void NavMeshDisplayer::addNavMeshModel(GeometryModel* model, PolygonMode polygonMode, const Vector3<float>& color) {
        model->setOutlineSize(4.0f);
        model->enableTransparency();
        model->setColor(color.X, color.Y, color.Z, 0.5f);
        model->setPolygonMode(polygonMode);
        navMeshModels.push_back(model);
        renderer3d->getGeometryManager()->addGeometry(model);
    }
}
