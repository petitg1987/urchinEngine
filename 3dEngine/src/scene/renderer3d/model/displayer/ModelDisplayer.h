#ifndef URCHINENGINE_MODELDISPLAYER_H
#define URCHINENGINE_MODELDISPLAYER_H

#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/displayer/DisplayMode.h"
#include "scene/renderer3d/camera/Camera.h"
#include "CustomModelShaderVariable.h"

namespace urchin {

    class ModelDisplayer : public Observer {
        public:
            explicit ModelDisplayer(Model *, const Matrix4<float>&, DisplayMode, std::shared_ptr<RenderTarget>, std::shared_ptr<Shader>);
            ~ModelDisplayer() override;

            void notify(Observable*, int) override;
            void onCameraProjectionUpdate(const Camera*);

            void display(const Matrix4<float>&, CustomModelShaderVariable*) const;

            void drawBBox(const Matrix4<float>&, const Matrix4<float>&) const;
            void drawBaseBones(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const;

        private:
            Model *model;
            DisplayMode displayMode;
            std::shared_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Shader> shader;
            ShaderVar mModelShaderVar, mNormalShaderVar, ambientFactorShaderVar, mViewShaderVar, mProjectionShaderVar;

            std::vector<std::unique_ptr<GenericRenderer>> meshRenderers;
    };

}

#endif
