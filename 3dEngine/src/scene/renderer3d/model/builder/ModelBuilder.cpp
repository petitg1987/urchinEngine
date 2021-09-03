#include <scene/renderer3d/model/builder/ModelBuilder.h>

namespace urchin {

    std::unique_ptr<Model> ModelBuilder::buildModel(const std::string& materialFilename, const std::vector<Point3<float>>& vertices,
                                                          const std::vector<unsigned int>& indices, const std::vector<Point2<float>>& uv) const {
        std::vector<std::unique_ptr<const ConstMesh>> constMeshesVector;
        constMeshesVector.push_back(buildConstMesh(materialFilename, vertices, indices, uv));
        auto constMeshes = std::make_shared<ConstMeshes>("???", std::move(constMeshesVector)); //TODO complete

        auto meshes = std::make_unique<Meshes>(constMeshes);
        return std::make_unique<Model>(std::move(meshes));
    }

    std::unique_ptr<const ConstMesh> ModelBuilder::buildConstMesh(const std::string& materialFilename, const std::vector<Point3<float>>& vertices,
                                                                        const std::vector<unsigned int>& indices, const std::vector<Point2<float>>& uv) const {
        if (vertices.size() != uv.size()) {
            throw std::runtime_error("Vertices (" + std::to_string(vertices.size()) + ") must have exactly one UV coordinate (" + std::to_string(uv.size()) + ")");
        } else if (indices.size() % 3 != 0) {
            throw std::runtime_error("Triangle indices must be a multiple of three");
        }

        std::vector<Vertex> modelVertices;
        modelVertices.reserve(vertices.size());
        std::vector<Weight> modelWeights;
        modelWeights.reserve(vertices.size());
        std::vector<Point2<float>> modelUvs;
        modelUvs.reserve(uv.size());

        unsigned int verticesGroupId = 0;
        int weightStart = 0;

        for (std::size_t i = 0;  i< vertices.size(); ++i) {
            Vertex modelVertex = {};
            modelVertex.linkedVerticesGroupId = verticesGroupId++;
            modelVertex.weightStart = weightStart++;
            modelVertex.weightCount = 1;
            modelVertices.push_back(modelVertex);

            modelUvs.push_back(uv[i]);

            Weight modelWeight = {};
            modelWeight.bone = 0;
            modelWeight.bias = 1;
            modelWeight.pos = vertices[i];
            modelWeights.push_back(modelWeight);
        }

        std::vector<Bone> modelBaseSkeleton;
        Bone modelBone = {};
        modelBone.name = "Bone";
        modelBone.parent = -1;
        modelBone.pos = Point3<float>(0.0f, 0.0f, 0.0f);
        modelBone.orient = Quaternion<float>();
        modelBaseSkeleton.push_back(modelBone);

        return std::make_unique<ConstMesh>(materialFilename, modelVertices, uv, indices, modelWeights, modelBaseSkeleton);
    }
    
}
