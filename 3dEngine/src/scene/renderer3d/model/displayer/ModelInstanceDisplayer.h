#pragma once

#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/DisplayMode.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/model/displayer/MeshFilter.h>
#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>
#include <resources/geometry/aabbox/AABBoxModel.h>
#include <resources/geometry/GeometryContainer.h>

namespace urchin {

    class ModelSetDisplayer;

    class ModelInstanceDisplayer {
        public:
            ModelInstanceDisplayer(const ModelSetDisplayer&, DisplayMode, RenderTarget&, const Shader&);
            ~ModelInstanceDisplayer();

            void setupCustomShaderVariable(CustomModelShaderVariable*);
            void setupDepthOperations(bool, bool);
            void setupFaceCull(bool);
            void setupBlendFunctions(const std::vector<BlendFunction>&);

            void initialize();

            void updateMeshVertices(const Model*) const;
            void updateMeshUv(const Model*) const;
            void updateMaterial(const Model*) const;
            void updateScale() const;

            const ModelSetDisplayer& getModelSetDisplayer() const;
            std::size_t getInstanceId() const;
            void updateInstanceId(std::size_t);
            std::span<Model* const> getInstanceModels() const;

            void addInstanceModel(Model&);
            void removeInstanceModel(Model&);
            unsigned int getInstanceCount() const;

            void resetRenderingModels();
            void registerRenderingModel(const Model&);
            void prepareRendering(unsigned int, const Matrix4<float>&, const MeshFilter*) const;

            void drawBBox(GeometryContainer&);
            void drawBaseBones(GeometryContainer&, const MeshFilter*) const;

        private:
            Model& getReferenceModel() const;

            void fillMaterialData(const Mesh&) const;
            std::vector<Point2<float>> scaleUv(const std::vector<Point2<float>>&, const std::vector<Vector3<float>>&, const UvScale&) const;
            TextureParam buildTextureParam(const Mesh&) const;

            static constexpr uint32_t PROJ_MATRIX_UNIFORM_BINDING = 0;
            static constexpr uint32_t MAT_DATA_UNIFORM_BINDING = 1;
            static constexpr uint32_t CUSTOM1_UNIFORM_BINDING = 2;
            static constexpr uint32_t CUSTOM2_UNIFORM_BINDING = 3;
            static constexpr uint32_t MAT_ALBEDO_UNIFORM_BINDING = 4;
            static constexpr uint32_t MAT_NORMAL_UNIFORM_BINDING = 5;
            static constexpr uint32_t MAT_ROUGHNESS_UNIFORM_BINDING = 6;
            static constexpr uint32_t MAT_METALNESS_UNIFORM_BINDING = 7;

            bool isInitialized;

            const ModelSetDisplayer& modelSetDisplayer;
            std::vector<Model*> instanceModels;
            std::size_t instanceId;
            DisplayMode displayMode;
            RenderTarget& renderTarget;
            const Shader& shader;

            mutable Matrix4<float> projectionViewMatrix;
            struct InstanceMatrix {
                Matrix4<float> modelMatrix;
                Matrix4<float> normalMatrix;
            };
            mutable std::vector<InstanceMatrix> instanceMatrices; //for DEFAULT_MODE only
            mutable std::vector<Matrix4<float>> instanceModelMatrices; //for DEPTH_ONLY_MODE only
            mutable struct {
                alignas(4) float encodedEmissiveFactor;
                alignas(4) float ambientFactor;
            } materialData;

            CustomModelShaderVariable* customShaderVariable;
            bool depthTestEnabled;
            bool depthWriteEnabled;
            bool enableFaceCull;
            std::vector<BlendFunction> blendFunctions;

            std::vector<std::unique_ptr<GenericRenderer>> meshRenderers;
            std::vector<std::shared_ptr<AABBoxModel>> aabboxModels;
    };

}
