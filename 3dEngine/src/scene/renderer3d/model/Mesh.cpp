#include <scene/renderer3d/model/Mesh.h>
#include <graphics/api/vulkan/render/GenericRendererBuilder.h>
#include <resources/model/MeshService.h>

namespace urchin {

    Mesh::Mesh(const ConstMesh& constMesh) :
            constMesh(constMesh),
            material(std::make_shared<Material>(constMesh.getInitialMaterial())) {

    }

    void Mesh::updateSkeleton(const std::vector<Bone>& skeleton) {
        MeshService::computeVertices(constMesh, skeleton, vertices);
        MeshService::computeNormalsAndTangents(constMesh, vertices, normals, tangents);
    }

    void Mesh::resetSkeleton() {
        MeshService::computeVertices(constMesh, constMesh.getBaseSkeleton(), vertices);
        MeshService::computeNormalsAndTangents(constMesh, vertices, normals, tangents);
    }

    void Mesh::updateVertices(const std::vector<Point3<float>>& vertices) {
        if (vertices.size() != constMesh.getNumberVertices()) {
            throw std::runtime_error("New vertices quantity (" + std::to_string(vertices.size()) + ") must be equals to original vertices quantity (" + std::to_string(constMesh.getNumberVertices()) + ")");
        }

        this->vertices = vertices;
        MeshService::computeNormalsAndTangents(constMesh, vertices, normals, tangents);
    }

    void Mesh::updateUv(const std::vector<Point2<float>>& uv) {
        if (uv.size() != constMesh.getNumberVertices()) {
            throw std::runtime_error("New UVs quantity (" + std::to_string(uv.size()) + ") must be equals to original vertices quantity (" + std::to_string(constMesh.getNumberVertices()) + ")");
        }

        this->uv = uv;
    }

    void Mesh::updateMaterial(std::shared_ptr<Material> material) {
        if (this->getMaterial().getUvScale() != material->getUvScale()) {
            //not supported because it would require detecting if material UV scale changed in ModelDisplayer#notify() method
            throw std::runtime_error("Update material with a different UV scale is not supported");
        }
        this->material = std::move(material);
    }

    const std::vector<Point3<float>>& Mesh::getVertices() const {
        if (vertices.empty()) {
            return constMesh.getBaseVertices();
        }
        return vertices;
    }

    const std::vector<Point2<float>>& Mesh::getUv() const {
        if (uv.empty()) {
            return constMesh.getUv();
        }
        return uv;
    }

    const std::vector<Vector3<float>>& Mesh::getNormals() const {
        if (normals.empty()) {
            return constMesh.getBaseNormals();
        }
        return normals;
    }

    const std::vector<Vector3<float>>& Mesh::getTangents() const {
        if (tangents.empty()) {
            return constMesh.getBaseTangents();
        }
        return tangents;
    }

    const Material& Mesh::getMaterial() const {
        return *material;
    }

    const std::shared_ptr<Material>& Mesh::getMaterialPtr() const {
        return material;
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
