#pragma once

#include <memory>

#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin {

    class Terrain;

    class TerrainObjectSpawner {
        public:
            explicit TerrainObjectSpawner(std::unique_ptr<Model>);

            void initialize(RenderTarget&, Terrain&);

            void onTerrainPositionUpdate();
            void onTerrainMeshUpdate();
            void onTerrainMaterialUpdate();
            void onTerrainLightingUpdate();
            void onTerrainViewDistanceUpdate();

            float getObjectsPerUnit() const;
            void setObjectsPerUnit(float);

            float getObjectsHeightShift() const;
            void setObjectsHeightShift(float);

            float getBaseDisplayDistance() const;
            void setBaseDisplayDistance(float);

            bool useTerrainLighting() const;
            void setUseTerrainLighting(bool);

            float getWindStrength() const;
            void setWindStrength(float);

            const Vector3<float>& getWindDirection() const;
            void setWindDirection(const Vector3<float>&);

            void prepareRendering(unsigned int, const Camera&, float);

        private:
            void updateProperties();

            void createOrRefreshObjectPositions();
            void createOrRefreshRenderers();
            void fillMeshData(const Mesh&);
            TextureParam buildTextureParam(const Mesh&) const;

            static constexpr uint32_t POSITIONING_DATA_UNIFORM_BINDING = 0;
            static constexpr uint32_t MESH_DATA_UNIFORM_BINDING = 1;
            static constexpr uint32_t CAMERA_INFO_UNIFORM_BINDING = 2;
            static constexpr uint32_t PROPERTIES_UNIFORM_BINDING = 3;
            static constexpr uint32_t MAT_ALBEDO_UNIFORM_BINDING = 4;
            static constexpr uint32_t MAT_NORMAL_UNIFORM_BINDING = 5;
            static constexpr uint32_t MAT_ROUGHNESS_UNIFORM_BINDING =6;
            static constexpr uint32_t MAT_METALNESS_UNIFORM_BINDING = 7;

            bool isInitialized;
            std::unique_ptr<Model> model;
            RenderTarget* renderTarget;
            Terrain* terrain;

            float objectsPerUnit;
            float objectsHeightShift;
            float baseDisplayDistance;
            struct {
                alignas(4) float displayDistance;
                alignas(4) bool useTerrainLighting;
                alignas(4) float windStrength;
                alignas(16) Vector3<float> windDirection;
            } properties;

            struct InstanceData {
                Matrix4<float> modelMatrix;
                Matrix4<float> normalMatrix;
                Vector3<float> terrainNormal;
            };
            std::vector<InstanceData> shaderInstanceData;

            struct {
                alignas(16) Matrix4<float> projectionView;
                alignas(16) Point3<float> cameraPosition;
                alignas(4) float sumTimeStep;
            } positioningData;

            struct {
                alignas(4) unsigned int lightMask;
                alignas(4) float encodedEmissiveFactor;
                alignas(4) float ambientFactor;
            } meshData;

            Vector2<float> jitterScale;
            struct CameraInfo {
                alignas(8) Vector2<float> jitterInPixel;
            } cameraInfo;

            std::unique_ptr<Shader> shader;
            std::vector<std::unique_ptr<GenericRenderer>> meshRenderers;
    };

}