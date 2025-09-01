#include <cstring>
#include <random>

#include "scene/renderer3d/landscape/terrain/object/TerrainObjectSpawner.h"
#include "scene/renderer3d/landscape/terrain/Terrain.h"
#include "graphics/render/shader/ShaderBuilder.h"
#include "graphics/render/GenericRendererBuilder.h"

namespace urchin {

    TerrainObjectSpawner::TerrainObjectSpawner(std::unique_ptr<Model> model) :
            isInitialized(false),
            model(std::move(model)),
            renderTarget(nullptr),
            terrain(nullptr),
            objectsPerUnit(1.0f),
            objectsHeightShift(0.0f),
            baseDisplayDistance(100.0f),
            properties({}),
            positioningData({}),
            meshData({}) {
        std::memset((void*)&properties, 0, sizeof(properties));
        std::memset((void*)&positioningData, 0, sizeof(positioningData));
        std::memset((void*)&meshData, 0, sizeof(meshData));
        std::memset((void*)&cameraInfo, 0, sizeof(cameraInfo));

        Vector3<float> axis; float angle;
        this->model->getTransform().getOrientation().toAxisAngle(axis, angle);
        if (!MathFunction::isZero(angle, 0.001f)) {
            throw std::runtime_error("Rotation on terrain object model is not supported yet");
        } else if (!MathFunction::isZero(this->model->getTransform().getPosition().toVector().length(), 0.001f)) {
            throw std::runtime_error("Translation on terrain object model is not supported yet");
        }

        properties.displayDistance = baseDisplayDistance;
        properties.useTerrainLighting = true;
        properties.windDirection = Vector3(1.0f, 0.0f, 0.0f);
        properties.windStrength = 0.5f;

        positioningData.sumTimeStep = 0.0f;

        cameraInfo.jitterInPixel = Vector2(0.0f, 0.0f);
    }

    void TerrainObjectSpawner::initialize(RenderTarget& renderTarget, Terrain& terrain) {
        assert(!isInitialized);
        this->renderTarget = &renderTarget;
        this->terrain = &terrain;

        createOrRefreshRenderers();

        isInitialized = true;
    }

    void TerrainObjectSpawner::onTerrainPositionUpdate() {
        createOrRefreshObjectPositions();
    }

    void TerrainObjectSpawner::onTerrainMeshUpdate() {
        createOrRefreshRenderers();
    }

    void TerrainObjectSpawner::onTerrainMaterialUpdate() {
        createOrRefreshRenderers();
    }

    void TerrainObjectSpawner::onTerrainLightingUpdate() {
        createOrRefreshRenderers();
    }

    void TerrainObjectSpawner::onTerrainViewDistanceUpdate() {
        updateProperties();
    }

    float TerrainObjectSpawner::getObjectsPerUnit() const {
        return objectsPerUnit;
    }

    void TerrainObjectSpawner::setObjectsPerUnit(float objectsPerUnit) {
        this->objectsPerUnit = objectsPerUnit;
        createOrRefreshObjectPositions();
    }

    float TerrainObjectSpawner::getObjectsHeightShift() const {
        return objectsHeightShift;
    }

    void TerrainObjectSpawner::setObjectsHeightShift(float objectsHeightShift) {
        this->objectsHeightShift = objectsHeightShift;
        createOrRefreshObjectPositions();
    }

    float TerrainObjectSpawner::getBaseDisplayDistance() const {
        return baseDisplayDistance;
    }

    void TerrainObjectSpawner::setBaseDisplayDistance(float baseDisplayDistance) {
        this->baseDisplayDistance = baseDisplayDistance;
        updateProperties();
    }

    bool TerrainObjectSpawner::useTerrainLighting() const {
        return properties.useTerrainLighting;
    }

    void TerrainObjectSpawner::setUseTerrainLighting(bool useTerrainLighting) {
        this->properties.useTerrainLighting = useTerrainLighting;
        updateProperties();
    }

    float TerrainObjectSpawner::getWindStrength() const {
        return properties.windStrength;
    }

    void TerrainObjectSpawner::setWindStrength(float windStrength) {
        this->properties.windStrength = windStrength;
        updateProperties();
    }

    const Vector3<float>& TerrainObjectSpawner::getWindDirection() const {
        return properties.windDirection;
    }

    void TerrainObjectSpawner::setWindDirection(const Vector3<float>& windDirection) {
        this->properties.windDirection = windDirection;
        updateProperties();
    }

    void TerrainObjectSpawner::updateProperties() {
        if (!terrain) {
            return;
        }

        properties.displayDistance = baseDisplayDistance * terrain->getObjectsViewDistancePercentage();
        for (auto& meshRenderer : meshRenderers) {
            meshRenderer->updateUniformData(PROPERTIES_UNIFORM_BINDING, &properties);
        }
    }

    void TerrainObjectSpawner::createOrRefreshRenderers() {
        createOrRefreshObjectPositions();

        constexpr float NDC_SPACE_TO_UV_COORDS_SCALE = 0.5f;
        jitterScale = Vector2((float)renderTarget->getWidth() * NDC_SPACE_TO_UV_COORDS_SCALE, (float)renderTarget->getHeight() * NDC_SPACE_TO_UV_COORDS_SCALE);

        shader = ShaderBuilder::createShader("terrainObject.vert.spv", "terrainObject.frag.spv", false);

        for (unsigned int i = 0; i < model->getMeshes()->getNumMeshes(); ++i) {
            const ConstMesh& constMesh = model->getConstMeshes()->getConstMesh(i);
            const Mesh& mesh = model->getMeshes()->getMesh(i);
            auto meshName =model->getMeshes()->getConstMeshes().getMeshesName();

            fillMeshData(mesh);

            auto meshRendererBuilder = GenericRendererBuilder::create("terrain obj - " + meshName, *renderTarget, *shader, ShapeType::TRIANGLE)
                    ->addData(mesh.getVertices())
                    ->addData(mesh.getUv())
                    ->addData(mesh.getNormals())
                    ->addData(mesh.getTangents())
                    ->indices(std::span(reinterpret_cast<const unsigned int*>(constMesh.getTrianglesIndices().data()), constMesh.getTrianglesIndices().size() * 3))
                    ->instanceData(shaderInstanceData.size(), {VariableType::MAT4_FLOAT, VariableType::MAT4_FLOAT, VariableType::VEC3_FLOAT}, (const float*)shaderInstanceData.data())
                    ->addUniformData(POSITIONING_DATA_UNIFORM_BINDING, sizeof(positioningData), &positioningData)
                    ->addUniformData(MESH_DATA_UNIFORM_BINDING, sizeof(meshData), &meshData)
                    ->addUniformData(CAMERA_INFO_UNIFORM_BINDING, sizeof(cameraInfo), &cameraInfo)
                    ->addUniformData(PROPERTIES_UNIFORM_BINDING, sizeof(properties), &properties)
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

        updateProperties();
    }

    void TerrainObjectSpawner::createOrRefreshObjectPositions() {
        if (!terrain) {
            return;
        }

        float startX = terrain->getMesh()->getVertices()[0].X;
        float endX = terrain->getMesh()->getVertices()[terrain->getMesh()->getVertices().size() - 1].X;
        float startZ = terrain->getMesh()->getVertices()[0].Z;
        float endZ = terrain->getMesh()->getVertices()[terrain->getMesh()->getVertices().size() - 1].Z;
        float sizeX = endX - startX;
        float sizeZ = endZ - startZ;
        float stepX = sizeX / (sizeX * objectsPerUnit);
        float stepZ = sizeZ / (sizeZ * objectsPerUnit);

        unsigned int seed = 0; //no need to generate different random numbers at each start
        float arbitraryDistribution = 1.0f / 3.0f;
        std::default_random_engine generator(seed);
        std::uniform_real_distribution xDistribution(-stepX * arbitraryDistribution, stepX * arbitraryDistribution);
        std::uniform_real_distribution zDistribution(-stepZ * arbitraryDistribution, stepZ * arbitraryDistribution);
        std::uniform_real_distribution rotationDistribution(0.0f, MathValue::PI_FLOAT * 2.0f);

        shaderInstanceData.clear();
        shaderInstanceData.reserve(MathFunction::ceilToUInt(sizeX / stepX) * MathFunction::ceilToUInt((sizeZ / stepZ)));
        for (float x = startX; x < endX; x += stepX) {
            for (float z = startZ; z < endZ; z += stepZ) {
                float xValue = x + xDistribution(generator);
                float zValue = z + zDistribution(generator);
                float yValue = terrain->getMesh()->findHeight(Point2(xValue, zValue)) + objectsHeightShift;

                Quaternion<float> orientation = Quaternion<float>::rotationY(rotationDistribution(generator));

                Vector3<float> normal = terrain->getMesh()->findNearestNormal(Point2(xValue, zValue));
                normal = (normal / 2.0f) + Vector3(0.5f, 0.5f, 0.5f);

                Point3 position = terrain->getPosition() + Point3(xValue, yValue, zValue);
                InstanceData instanceData {
                    .modelMatrix = Transform(position, orientation, this->model->getTransform().getScale()).getTransformMatrix(),
                    .normalMatrix = instanceData.modelMatrix.inverse().transpose(),
                    .terrainNormal = normal
                };
                shaderInstanceData.push_back(instanceData);
            }
        }

        for (auto& meshRenderer : meshRenderers) {
            meshRenderer->updateInstanceData(POSITIONING_DATA_UNIFORM_BINDING, (const float*)shaderInstanceData.data());
        }
    }

    void TerrainObjectSpawner::fillMeshData(const Mesh& mesh) {
        //model properties
        meshData.lightMask = terrain->getLightMask();

        //material
        if (properties.useTerrainLighting) {
            meshData.encodedEmissiveFactor = std::clamp(terrain->getMaterials()->getMaterials()[0]->getEmissiveFactor() / Material::MAX_EMISSIVE_FACTOR, 0.0f, 1.0f);
            meshData.ambientFactor = terrain->getAmbient();
        } else {
            meshData.encodedEmissiveFactor = std::clamp(mesh.getMaterial().getEmissiveFactor() / Material::MAX_EMISSIVE_FACTOR, 0.0f, 1.0f);
            meshData.ambientFactor = mesh.getMaterial().getAmbientFactor();
        }

        //model data
        meshData.modelHeight = model->getLocalAABBox().getMax().Y - model->getLocalAABBox().getMin().Y;
        meshData.modelMinY = model->getLocalAABBox().getMin().Y;
    }

    TextureParam TerrainObjectSpawner::buildTextureParam(const Mesh& mesh) const {
        TextureParam::ReadMode textureReadMode = mesh.getMaterial().repeatTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
        return TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
    }

    void TerrainObjectSpawner::prepareRendering(unsigned int renderingOrder, const Camera& camera, float dt) {
        positioningData.projectionView = camera.getProjectionViewMatrix();
        positioningData.cameraPosition = camera.getPosition();
        positioningData.sumTimeStep += dt;

        cameraInfo.jitterInPixel = camera.getAppliedJitter() * jitterScale;

        for (auto& meshRenderer : meshRenderers) {
            meshRenderer->updateUniformData(POSITIONING_DATA_UNIFORM_BINDING, &positioningData);
            meshRenderer->updateUniformData(CAMERA_INFO_UNIFORM_BINDING, &cameraInfo);

            meshRenderer->enableRenderer(renderingOrder);
        }
    }

}