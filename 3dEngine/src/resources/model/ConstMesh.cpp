#include <stdexcept>
#include <limits>
#include <utility>

#include <resources/model/ConstMesh.h>
#include <resources/model/MeshService.h>

namespace urchin {

    ConstMesh::ConstMesh(std::shared_ptr<Material> initialMaterial, const std::vector<Vertex>& vertices, std::vector<Point2<float>> uvTexture,
            std::vector<unsigned int> trianglesIndices, std::vector<Weight> weights, const std::vector<Bone>& baseSkeleton) :
            initialMaterial(std::move(initialMaterial)),
            vertices(vertices),
            uvTexture(std::move(uvTexture)),
            trianglesIndices(std::move(trianglesIndices)),
            weights(std::move(weights)),
            baseSkeleton(baseSkeleton) {

        //regroup duplicate vertex due to their different texture coordinates
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            linkedVertices[vertices[i].linkedVerticesGroupId].push_back((unsigned int)i);
        }

        //compute vertices and normals based on bind-pose skeleton
        MeshService::computeVertices(*this, baseSkeleton, baseVertices);
        MeshService::computeNormalsAndTangents(*this, baseVertices, baseNormals, baseTangents);

        //determine used bones
        for (const Weight& weight : this->weights) {
            if (std::ranges::find(usedBonesIndices, weight.boneIndex) == usedBonesIndices.end()) {
                usedBonesIndices.push_back(weight.boneIndex);
            }
        }
    }

    const Material& ConstMesh::getInitialMaterial() const {
        return *initialMaterial;
    }

    unsigned int ConstMesh::getNumberVertices() const {
        return (unsigned int)vertices.size();
    }

    const Vertex& ConstMesh::getStructVertex(unsigned int index) const {
        return vertices[index];
    }

    const std::vector<Point2<float>>& ConstMesh::getUvTexture() const {
        return uvTexture;
    }

    /**
     * Vertices can be duplicated because they have different texture coordinates.
     * This method returns all duplicates vertices thanks to 'linked vertices group ID' stored on each vertex.
     */
    const std::vector<unsigned int>& ConstMesh::getLinkedVertices(unsigned int linkedVerticesGroupId) const {
        auto it = linkedVertices.find(linkedVerticesGroupId);
        if (it != linkedVertices.end()) {
            return it->second;
        }

        throw std::runtime_error("Impossible to find linked vertices for group ID: " + std::to_string(linkedVerticesGroupId));
    }

    const std::vector<unsigned int>& ConstMesh::getTrianglesIndices() const {
        return trianglesIndices;
    }

    unsigned int ConstMesh::getNumberWeights() const {
        return (unsigned int)weights.size();
    }

    const Weight& ConstMesh::getWeight(unsigned int index) const {
        return weights[index];
    }

    const std::vector<std::size_t>& ConstMesh::getUsedBonesIndices() const {
        return usedBonesIndices;
    }

    unsigned int ConstMesh::getNumberBones() const {
        return (unsigned int)baseSkeleton.size();
    }

    const std::vector<Bone>& ConstMesh::getBaseSkeleton() const {
        return baseSkeleton;
    }

    const Bone& ConstMesh::getBaseBone(unsigned int index) const {
        return baseSkeleton[index];
    }

    const std::vector<Point3<float>>& ConstMesh::getBaseVertices() const {
        return baseVertices;
    }

    const std::vector<Vector3<float>>& ConstMesh::getBaseNormals() const {
        return baseNormals;
    }

    const std::vector<Vector3<float>>& ConstMesh::getBaseTangents() const {
        return baseTangents;
    }

}
