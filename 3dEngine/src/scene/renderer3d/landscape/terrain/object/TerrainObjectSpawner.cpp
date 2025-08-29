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
    }

    void TerrainObjectSpawner::initialize(RenderTarget& renderTarget, TerrainMesh& terrainMesh, const Point3<float>& terrainPosition) {
        assert(!isInitialized);
        this->renderTarget = &renderTarget;
        this->terrainMesh = &terrainMesh;
        this->terrainPosition = terrainPosition;

        generateObjectPositions();
        
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
                    ->instanceData(shaderInstanceData.size(), {VariableType::MAT4_FLOAT, VariableType::MAT4_FLOAT}, (const float*)shaderInstanceData.data())
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
            }

            meshRenderers.push_back(meshRendererBuilder->build());
        }

        isInitialized = true;
    }

    void TerrainObjectSpawner::generateObjectPositions() {
        constexpr float objectsPerUnit = 0.5f;
        constexpr float heightDelta = 0.5f;

        float startX = terrainMesh->getVertices()[0].X;
        float endX = terrainMesh->getVertices()[terrainMesh->getVertices().size() - 1].X;
        float startZ = terrainMesh->getVertices()[0].Z;
        float endZ = terrainMesh->getVertices()[terrainMesh->getVertices().size() - 1].Z;
        float sizeX = endX - startX;
        float sizeZ = endZ - startZ;

        const Transform<float>& baseTransform = this->model->getTransform();
        //TODO reserve shaderInstanceData
        float stepX = sizeX / (sizeX * objectsPerUnit);
        float stepZ = sizeZ / (sizeZ * objectsPerUnit);
        shaderInstanceData.clear();
        shaderInstanceData.reserve(MathFunction::ceilToUInt(sizeX / stepX) * MathFunction::ceilToUInt((sizeZ / stepZ)));
        for (float x = startX; x < endX; x += stepX) {
            for (float z = startZ; z < endZ; z += stepZ) {
                float y = terrainMesh->findHeightAt(Point2(x, z)) + heightDelta;
                Point3 position(x, y, z);

                InstanceData instanceData {
                    .modelMatrix = Transform(terrainPosition + position, baseTransform.getOrientation(), baseTransform.getScale()).getTransformMatrix(),
                    .normalMatrix = instanceData.modelMatrix.inverse().transpose() //TODO always the same: not instance !
                };
                shaderInstanceData.push_back(instanceData);
            }
        }
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
          //  meshRenderer->updateInstanceData(shaderInstanceData.size(), (const float*) shaderInstanceData.data()); //TODO useless ?
            meshRenderer->updateUniformData(PROJ_VIEW_MATRIX_UNIFORM_BINDING, &camera.getProjectionViewMatrix());

            cameraInfo.jitterInPixel = camera.getAppliedJitter() * jitterScale;
            meshRenderer->updateUniformData(CAMERA_INFO_UNIFORM_BINDING, &cameraInfo);

            meshRenderer->enableRenderer(renderingOrder);
        }
    }

}