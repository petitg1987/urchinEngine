#ifndef URCHINENGINE_MODELDISPLAYER_H
#define URCHINENGINE_MODELDISPLAYER_H

#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/DisplayMode.h>
#include <scene/renderer3d/camera/Camera.h>
#include <resources/geometry/aabbox/AABBoxModel.h>
#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>

namespace urchin {

    class ModelDisplayer : public Observer {
        public:
            ModelDisplayer(Model *, const Matrix4<float>&, DisplayMode, std::shared_ptr<RenderTarget>, std::shared_ptr<Shader>, CustomModelShaderVariable*);
            ~ModelDisplayer() override;

            void notify(Observable*, int) override;
            void onCameraProjectionUpdate(const Camera*);

            void prepareRendering(const Matrix4<float>&) const;

            void drawBBox(const Matrix4<float>&, const Matrix4<float>&);
            void drawBaseBones(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const;

        private:
            Model *model;
            DisplayMode displayMode;
            std::shared_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Shader> shader;
            mutable struct {
                alignas(16) Matrix4<float> viewMatrix;
                alignas(16) Matrix4<float> modelMatrix;
                alignas(16) Matrix4<float> normalMatrix;
                alignas(4) float ambientFactor;
            } meshData;
            CustomModelShaderVariable* customModelShaderVariable;

            std::vector<std::shared_ptr<GenericRenderer>> meshRenderers;
            std::unique_ptr<AABBoxModel> aabboxModel;
    };

}

#endif
