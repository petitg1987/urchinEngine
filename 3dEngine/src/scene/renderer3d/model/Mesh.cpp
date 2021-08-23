#include <scene/renderer3d/model/Mesh.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <resources/model/MeshService.h>

namespace urchin {

    Mesh::Mesh(const ConstMesh& constMesh) :
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

    void Mesh::drawBaseBones(GeometryContainer& geometryContainer, const Matrix4<float>& modelMatrix) {
        if (boneSphereModels) {
            geometryContainer.removeGeometry(*boneSphereModels);
        }

        std::vector<Sphere<float>> sphereBonePoints;
        sphereBonePoints.reserve(constMesh.getBaseSkeleton().size());
        for (const auto& bone : constMesh.getBaseSkeleton()) {
            sphereBonePoints.emplace_back(0.05f, bone.pos);
        }
        boneSphereModels = std::make_shared<SphereModel>(sphereBonePoints, 7);
        boneSphereModels->setAlwaysVisible(true);
        boneSphereModels->setPolygonMode(PolygonMode::FILL);
        boneSphereModels->setModelMatrix(modelMatrix);
        geometryContainer.addGeometry(boneSphereModels);
    }

}
