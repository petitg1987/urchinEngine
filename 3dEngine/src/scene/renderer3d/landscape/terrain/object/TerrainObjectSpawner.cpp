#include <cstring>

#include "scene/renderer3d/landscape/terrain/object/TerrainObjectSpawner.h"
#include "graphics/render/shader/ShaderBuilder.h"
#include "graphics/render/GenericRendererBuilder.h"

namespace urchin {

    TerrainObjectSpawner::TerrainObjectSpawner(TerrainMesh& terrainMesh, std::unique_ptr<Model> model) :
            terrainMesh(terrainMesh),
            model(std::move(model)),
            isInitialized(false),
            renderTarget(nullptr),
            meshData({}) {
        std::memset((void*)&meshData, 0, sizeof(meshData));
        std::memset((void*)&cameraInfo, 0, sizeof(cameraInfo));

        cameraInfo.jitterInPixel = Vector2(0.0f, 0.0f);

        InstanceMatrix instanceMatrix;
        instanceMatrix.modelMatrix = this->model->getTransform().getTransformMatrix();
        instanceMatrix.normalMatrix = instanceMatrix.modelMatrix.inverse().transpose();
        instanceMatrices.push_back(instanceMatrix);
    }

    void TerrainObjectSpawner::initialize(RenderTarget& renderTarget) {
        assert(!isInitialized);
        this->renderTarget = &renderTarget;

        shader = ShaderBuilder::createShader("terrainObject.vert.spv", "terrainObject.frag.spv", false);

        for (unsigned int i = 0; i < model->getMeshes()->getNumMeshes(); ++i) {
            const ConstMesh& constMesh = model->getConstMeshes()->getConstMesh(i);
            const Mesh& mesh = model->getMeshes()->getMesh(i);
            auto meshName =model->getMeshes()->getConstMeshes().getMeshesName();

            Matrix4<float> projectionViewMatrix;
            fillMeshData(mesh);

            auto meshRendererBuilder = GenericRendererBuilder::create("terrain obj - " + meshName, renderTarget, *shader, ShapeType::TRIANGLE)
                    ->addData(mesh.getVertices())
                    ->indices(std::span(reinterpret_cast<const unsigned int*>(constMesh.getTrianglesIndices().data()), constMesh.getTrianglesIndices().size() * 3))
                    ->addUniformData(PROJ_VIEW_MATRIX_UNIFORM_BINDING, sizeof(projectionViewMatrix), &projectionViewMatrix)
                    ->addUniformData(MESH_DATA_UNIFORM_BINDING, sizeof(meshData), &meshData)
                    ->addUniformData(CAMERA_INFO_UNIFORM_BINDING, sizeof(cameraInfo), &cameraInfo);

            instanceMatrices.emplace_back(InstanceMatrix{.modelMatrix = Matrix4<float>(), .normalMatrix = Matrix4<float>()});
            meshRendererBuilder->instanceData(instanceMatrices.size(), {VariableType::MAT4_FLOAT, VariableType::MAT4_FLOAT}, (const float*)instanceMatrices.data());

            // if (!enableFaceCull) {
            //     meshRendererBuilder->disableCullFace();
            // }
            // if (!blendFunctions.empty()) {
            //     meshRendererBuilder->enableTransparency(blendFunctions);
            // }

            meshRendererBuilder
                    ->addData(mesh.getUv())
                    ->addData(mesh.getNormals())
                    ->addData(mesh.getTangents())
                    ->addUniformTextureReader(MAT_ALBEDO_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getAlbedoTexture(), buildTextureParam(mesh)))
                    ->addUniformTextureReader(MAT_NORMAL_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh)))
                    ->addUniformTextureReader(MAT_ROUGHNESS_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getRoughnessTexture(), buildTextureParam(mesh)))
                    ->addUniformTextureReader(MAT_METALNESS_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getMetalnessTexture(), buildTextureParam(mesh)));

            meshRenderers.push_back(meshRendererBuilder->build());
        }

        isInitialized = true;
    }

    void TerrainObjectSpawner::fillMeshData(const Mesh& mesh) {
        //model properties
        meshData.lightMask = model->getLightMask();

        //material
        meshData.encodedEmissiveFactor = std::clamp(mesh.getMaterial().getEmissiveFactor() / Material::MAX_EMISSIVE_FACTOR, 0.0f, 1.0f);
        meshData.ambientFactor = mesh.getMaterial().getAmbientFactor();
    }

    TextureParam TerrainObjectSpawner::buildTextureParam(const Mesh& mesh) const {
        TextureParam::ReadMode textureReadMode = mesh.getMaterial().repeatTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
        return TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
    }

    void TerrainObjectSpawner::prepareRendering(unsigned int renderingOrder, const Camera& camera, float /*dt*/) {
        for (auto& meshRenderer : meshRenderers) {
            meshRenderer->updateInstanceData(instanceMatrices.size(), (const float*) instanceMatrices.data());
            meshRenderer->updateUniformData(PROJ_VIEW_MATRIX_UNIFORM_BINDING, &camera.getProjectionViewMatrix());

            constexpr float NDC_SPACE_TO_UV_COORDS_SCALE = 0.5f;
            float renderingSceneWidth = (float)renderTarget->getWidth();
            float renderingSceneHeight = (float)renderTarget->getHeight();
            cameraInfo.jitterInPixel = camera.getAppliedJitter() * Vector2(renderingSceneWidth * NDC_SPACE_TO_UV_COORDS_SCALE, renderingSceneHeight * NDC_SPACE_TO_UV_COORDS_SCALE);
            meshRenderer->updateUniformData(CAMERA_INFO_UNIFORM_BINDING, &cameraInfo);

            meshRenderer->enableRenderer(renderingOrder);
        }
    }

}