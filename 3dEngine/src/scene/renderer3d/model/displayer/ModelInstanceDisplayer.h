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

    class ModelInstanceDisplayer : public Observer {
        public:
            ModelInstanceDisplayer(const ModelSetDisplayer&, DisplayMode, RenderTarget&, const Shader&);
            ~ModelInstanceDisplayer() override;

            void setupCustomShaderVariable(CustomModelShaderVariable*);
            void setupDepthOperations(bool, bool);
            void setupFaceCull(bool);
            void setupBlendFunctions(const std::vector<BlendFunction>&);

            void initialize();
            void notify(Observable*, int) override;

            const ModelSetDisplayer& getModelSetDisplayer() const;
            std::size_t getInstanceId() const;

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

            void fillMaterialData(const Mesh&);
            std::vector<Point2<float>> scaleUv(const std::vector<Point2<float>>&, const std::vector<Vector3<float>>&, const UvScale&) const;
            TextureParam buildTextureParam(const Mesh&) const;

            bool checkUpdateAllowance(const Model*) const;
            void updateMesh(const Model*);
            void updateMaterial(const Model*);
            void updateScale(const Model*);

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
            mutable std::vector<InstanceMatrix> instanceMatrices;
            mutable struct {
                alignas(4) float encodedEmissiveFactor;
                alignas(4) float ambientFactor;
            } materialData;

            CustomModelShaderVariable* customShaderVariable;
            bool depthTestEnabled, depthWriteEnabled;
            bool enableFaceCull;
            std::vector<BlendFunction> blendFunctions;

            std::vector<std::unique_ptr<GenericRenderer>> meshRenderers;
            std::vector<std::shared_ptr<AABBoxModel>> aabboxModels;
    };

}
