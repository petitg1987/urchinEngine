#include "scene/renderer3d/model/builder/ModelBuilder.h"
#include "resources/ResourceRetriever.h"
#include "resources/material/MaterialBuilder.h"

namespace urchin {

    //static
    unsigned long ModelBuilder::nextId = 0;

    ModelBuilder::ModelBuilder() {
        std::vector<unsigned char> defaultAlbedoColor({0, 0, 0, 255});
        std::shared_ptr<Texture> albedoTexture = Texture::build("defaultAlbedo", 1, 1, TextureFormat::RGBA_8_UINT_NORM, defaultAlbedoColor.data(), TextureDataType::INT_8);
        material = MaterialBuilder::create("defaultMaterial", std::move(albedoTexture), false)->build();
    }

    ModelBuilder::ModelBuilder(std::shared_ptr<Material> material) :
            material(std::move(material)) {

    }

    ModelBuilder::ModelBuilder(const std::string& materialFilename) :
            material(ResourceRetriever::instance().getResource<Material>(materialFilename, {})) {

    }

    std::unique_ptr<Model> ModelBuilder::newEmptyModel(const std::string& meshesName) const {
        std::vector vertices = {Point3(0.1f, 0.0f, 0.0f), Point3(0.2f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 0.0f)};
        std::vector<std::array<uint32_t, 3>> triangleIndices = {{0u, 1u, 2u}};
        std::vector uvTexture = {Point2(0.0f, 0.0f), Point2(0.0f, 0.0f), Point2(0.0f, 0.0f)};

        return newModel(meshesName, vertices, triangleIndices, uvTexture);
    }

    std::unique_ptr<Model> ModelBuilder::newModel(const std::string& meshesName, const std::vector<Point3<float>>& vertices,
                                                  const std::vector<std::array<uint32_t, 3>>& trianglesIndices, const std::vector<Point2<float>>& uvTexture) const {
        std::vector<std::unique_ptr<const ConstMesh>> constMeshesVector;
        constMeshesVector.push_back(buildConstMesh(vertices, trianglesIndices, uvTexture));
        auto constMeshes = ConstMeshes::fromMemory(meshesName, std::move(constMeshesVector));
        constMeshes->setId("_" + std::to_string(nextId++));

        auto meshes = std::make_unique<Meshes>(std::move(constMeshes));
        return Model::fromMemory(std::move(meshes));
    }

    std::unique_ptr<const ConstMesh> ModelBuilder::buildConstMesh(const std::vector<Point3<float>>& vertices, const std::vector<std::array<uint32_t, 3>>& trianglesIndices,
            const std::vector<Point2<float>>& uvTexture) const {
        if (vertices.size() != uvTexture.size()) {
            throw std::runtime_error("Vertices (" + std::to_string(vertices.size()) + ") must have exactly one UV coordinate (" + std::to_string(uvTexture.size()) + ")");
        }

        std::vector<Vertex> modelVertices;
        modelVertices.reserve(vertices.size());
        std::vector<Weight> modelWeights;
        modelWeights.reserve(vertices.size());
        std::vector<Point2<float>> modelUvs;
        modelUvs.reserve(uvTexture.size());

        unsigned int verticesGroupId = 0;
        int weightStart = 0;

        for (std::size_t i = 0;  i< vertices.size(); ++i) {
            Vertex modelVertex = {};
            modelVertex.linkedVerticesGroupId = verticesGroupId++;
            modelVertex.weightStart = weightStart++;
            modelVertex.weightCount = 1;
            modelVertices.push_back(modelVertex);

            modelUvs.push_back(uvTexture[i]);

            Weight modelWeight = {};
            modelWeight.boneIndex = 0;
            modelWeight.bias = 1;
            modelWeight.pos = vertices[i];
            modelWeights.push_back(modelWeight);
        }

        std::vector<Bone> modelBaseSkeleton;
        Bone modelBone = {};
        modelBone.name = "Bone";
        modelBone.parent = -1;
        modelBone.pos = Point3(0.0f, 0.0f, 0.0f);
        modelBone.orient = Quaternion<float>();
        modelBaseSkeleton.push_back(modelBone);

        return std::make_unique<ConstMesh>(material, modelVertices, uvTexture, trianglesIndices, modelWeights, modelBaseSkeleton);
    }
    
}
