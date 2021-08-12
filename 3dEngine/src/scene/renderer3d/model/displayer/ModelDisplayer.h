#pragma once

#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/DisplayMode.h>
#include <scene/renderer3d/camera/Camera.h>
#include <resources/geometry/aabbox/AABBoxModel.h>
#include <scene/renderer3d/model/displayer/MeshFilter.h>
#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>

namespace urchin {

    class ModelDisplayer : public Observer {
        public:
            ModelDisplayer(Model *, const Matrix4<float>&, DisplayMode, RenderTarget&, const Shader&);
            ~ModelDisplayer() override;

            void setupCustomShaderVariable(CustomModelShaderVariable*);
            void setupDepthOperations(bool, bool);
            void setupFaceCull(bool);
            void setupBlendFunctions(const std::vector<BlendFunction>&);

            void initialize();
            void notify(Observable*, int) override;
            void onCameraProjectionUpdate(const Camera&);

            void prepareRendering(const Matrix4<float>&, const MeshFilter*) const;

            void drawBBox(const Matrix4<float>&, const Matrix4<float>&);
            void drawBaseBones(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix, const MeshFilter* meshFilter) const;

        private:
            bool isInitialized;

            Model *model;
            Matrix4<float> projectionMatrix;
            DisplayMode displayMode;
            RenderTarget& renderTarget;
            const Shader& shader;
            mutable struct {
                alignas(16) Matrix4<float> viewMatrix;
                alignas(16) Matrix4<float> modelMatrix;
            } positioningData;
            mutable struct {
                alignas(16) Matrix4<float> normalMatrix;
                alignas(4) float ambientFactor;
            } meshData;

            CustomModelShaderVariable* customShaderVariable;
            bool depthTestEnabled, depthWriteEnabled;
            bool enableFaceCull;
            std::vector<BlendFunction> blendFunctions;

            std::vector<std::unique_ptr<GenericRenderer>> meshRenderers;
            std::unique_ptr<AABBoxModel> aabboxModel;
    };

}
