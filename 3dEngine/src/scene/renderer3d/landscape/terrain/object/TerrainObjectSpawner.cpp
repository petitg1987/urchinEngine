#include <cstring>

#include "scene/renderer3d/landscape/terrain/object/TerrainObjectSpawner.h"
#include "graphics/render/shader/ShaderBuilder.h"
#include "graphics/render/GenericRendererBuilder.h"

namespace urchin {

    TerrainObjectSpawner::TerrainObjectSpawner(std::unique_ptr<Model> model) :
            isInitialized(false),
            model(std::move(model)),
            renderTarget(nullptr),
            terrainMesh(nullptr),
            meshData({}) {
        std::memset((void*)&meshData, 0, sizeof(meshData));
        std::memset((void*)&cameraInfo, 0, sizeof(cameraInfo));

        cameraInfo.jitterInPixel = Vector2(0.0f, 0.0f);

        InstanceMatrix instanceMatrix;
        instanceMatrix.modelMatrix = this->model->getTransform().getTransformMatrix();
        instanceMatrix.normalMatrix = instanceMatrix.modelMatrix.inverse().transpose();
        instanceMatrices.push_back(instanceMatrix);
    }

    void TerrainObjectSpawner::initialize(RenderTarget& renderTarget, TerrainMesh& terrainMesh) {
        assert(!isInitialized);
        this->renderTarget = &renderTarget;
        this->terrainMesh = &terrainMesh;

        constexpr float NDC_SPACE_TO_UV_COORDS_SCALE = 0.5f;
        jitterScale = Vector2((float)renderTarget.getWidth() * NDC_SPACE_TO_UV_COORDS_SCALE, (float)renderTarget.getHeight() * NDC_SPACE_TO_UV_COORDS_SCALE);

        shader = ShaderBuilder::createShader("terrainObject.vert.spv", "terrainObject.frag.spv", false);

        for (unsigned int i = 0; i < model->getMeshes()->getNumMeshes(); ++i) {
            const ConstMesh& constMesh = model->getConstMeshes()->getConstMesh(i);
            const Mesh& mesh = model->getMeshes()->getMesh(i);
            auto meshName =model->getMeshes()->getConstMeshes().getMeshesName();

            Matrix4<float> projectionViewMatrix;
            fillMeshData(mesh);

            auto meshRendererBuilder = GenericRendererBuilder::create("terrain obj - " + meshName, renderTarget, *shader, ShapeType::TRIANGLE)
                    ->addData(mesh.getVertices())
                    ->addData(mesh.getUv())
                    ->addData(mesh.getNormals())
                    ->addData(mesh.getTangents())
                    ->indices(std::span(reinterpret_cast<const unsigned int*>(constMesh.getTrianglesIndices().data()), constMesh.getTrianglesIndices().size() * 3))
                    ->instanceData(instanceMatrices.size(), {VariableType::MAT4_FLOAT, VariableType::MAT4_FLOAT}, (const float*)instanceMatrices.data())
                    ->addUniformData(PROJ_VIEW_MATRIX_UNIFORM_BINDING, sizeof(projectionViewMatrix), &projectionViewMatrix)
                    ->addUniformData(MESH_DATA_UNIFORM_BINDING, sizeof(meshData), &meshData)
                    ->addUniformData(CAMERA_INFO_UNIFORM_BINDING, sizeof(cameraInfo), &cameraInfo)
                    ->addUniformTextureReader(MAT_ALBEDO_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getAlbedoTexture(), buildTextureParam(mesh)))
                    ->addUniformTextureReader(MAT_NORMAL_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh)))
                    ->addUniformTextureReader(MAT_ROUGHNESS_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getRoughnessTexture(), buildTextureParam(mesh)))
                    ->addUniformTextureReader(MAT_METALNESS_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getMetalnessTexture(), buildTextureParam(mesh)));

            if (mesh.getMaterial().isDepthTestEnabled()) {
                meshRendererBuilder->enableDepthTest();
            }
            if (mesh.getMaterial().isDepthWriteEnabled()) {
                meshRendererBuilder->enableDepthWrite();
            }
            if (mesh.getMaterial().getAlbedoTexture()->hasTransparency()) {
                meshRendererBuilder->disableCullFace();
                meshRendererBuilder->enableTransparency({BlendFunction::buildDefault()});
            }

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

            cameraInfo.jitterInPixel = camera.getAppliedJitter() * jitterScale;
            meshRenderer->updateUniformData(CAMERA_INFO_UNIFORM_BINDING, &cameraInfo);

            meshRenderer->enableRenderer(renderingOrder);
        }
    }

}