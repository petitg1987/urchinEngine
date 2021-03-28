#include "Mesh.h"
#include "resources/model/MeshService.h"
#include "resources/geometry/points/PointsModel.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    Mesh::Mesh(const ConstMesh* constMesh) :
            constMesh(constMesh) {

    }

    void Mesh::update(const std::vector<Bone>& skeleton) {
        //recompute the vertices, normals and tangents
        MeshService::computeVertices(constMesh, skeleton, vertices);
        MeshService::computeNormalsAndTangents(constMesh, vertices, normals, tangents);
    }

    const std::vector<Point3<float>>& Mesh::getVertices() const {
        return vertices;
    }

    const std::vector<Vector3<float>>& Mesh::getNormals() const {
        return normals;
    }

    const std::vector<Vector3<float>>& Mesh::getTangents() const {
        return tangents;
    }

    void Mesh::drawBaseBones(const std::shared_ptr<RenderTarget>& renderTarget, const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const {
        std::vector<Point3<float>> bonePositions;
        for (const auto& bone : constMesh->getBaseSkeleton()) {
            bonePositions.push_back(bone.pos);
        }

        std::unique_ptr<PointsModel> pointsModel = std::make_unique<PointsModel>(bonePositions);
        pointsModel->initialize(renderTarget);
        pointsModel->setOutlineSize(5.0f);
        pointsModel->onCameraProjectionUpdate(projectionMatrix);
        pointsModel->setAlwaysVisible(true);
        pointsModel->prepareRendering(viewMatrix);
    }

}
