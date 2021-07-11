#pragma once

#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/DisplayMode.h>
#include <scene/renderer3d/camera/Camera.h>
#include <resources/geometry/aabbox/AABBoxModel.h>
#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>

namespace urchin {

    class ModelDisplayer : public Observer {
        public:
            ModelDisplayer(Model *, const Matrix4<float>&, DisplayMode, RenderTarget&, const Shader&, CustomModelShaderVariable*);
            ~ModelDisplayer() override;

            void notify(Observable*, int) override;
            void onCameraProjectionUpdate(const Camera*);

            void prepareRendering(const Matrix4<float>&) const;

            void drawBBox(const Matrix4<float>&, const Matrix4<float>&);
            void drawBaseBones(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const;

        private:
            Model *model;
            DisplayMode displayMode;
            RenderTarget& renderTarget;
            const Shader& shader;
            mutable struct {
                alignas(16) Matrix4<float> viewMatrix;
                alignas(16) Matrix4<float> modelMatrix;
                alignas(16) Matrix4<float> normalMatrix;
                alignas(4) float ambientFactor;
            } meshData;
            CustomModelShaderVariable* customModelShaderVariable;

            std::vector<std::unique_ptr<GenericRenderer>> meshRenderers;
            std::unique_ptr<AABBoxModel> aabboxModel;
    };

}
