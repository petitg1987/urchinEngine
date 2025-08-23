#pragma once

#include <memory>

#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/landscape/terrain/TerrainMesh.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin {

    class TerrainObjectSpawner {
        public:
            TerrainObjectSpawner(TerrainMesh&, std::unique_ptr<Model>);

            void prepareRendering(unsigned int, const Camera&, float);

        private:
            void initialize(RenderTarget&);
            void fillMeshData(const Mesh&);
            TextureParam buildTextureParam(const Mesh&) const;

            static constexpr uint32_t PROJ_VIEW_MATRIX_UNIFORM_BINDING = 0;
            static constexpr uint32_t MESH_DATA_UNIFORM_BINDING = 1;
            static constexpr uint32_t CAMERA_INFO_UNIFORM_BINDING = 2;
            static constexpr uint32_t MAT_ALBEDO_UNIFORM_BINDING = 3;
            static constexpr uint32_t MAT_NORMAL_UNIFORM_BINDING = 4;
            static constexpr uint32_t MAT_ROUGHNESS_UNIFORM_BINDING = 5;
            static constexpr uint32_t MAT_METALNESS_UNIFORM_BINDING = 6;

            TerrainMesh& terrainMesh;
            std::unique_ptr<Model> model;

            bool isInitialized;
            RenderTarget* renderTarget;

            struct InstanceMatrix {
                Matrix4<float> modelMatrix;
                Matrix4<float> normalMatrix;
            };
            std::vector<InstanceMatrix> instanceMatrices;
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