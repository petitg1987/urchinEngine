#ifndef URCHINENGINE_MODELDISPLAYER_H
#define URCHINENGINE_MODELDISPLAYER_H

#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/displayer/DisplayMode.h"

namespace urchin {

    class ModelDisplayer : public Observer {
        public:
            explicit ModelDisplayer(Model *, DisplayMode, std::shared_ptr<RenderTarget>, std::shared_ptr<Shader>);
            ~ModelDisplayer() override;

            void notify(Observable*, int) override;

            void display(const MeshParameter&) const;

            void drawBBox(const Matrix4<float>&, const Matrix4<float>&) const;
            void drawBaseBones(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const;

        private:
            Model *model;
            DisplayMode displayMode;
            std::shared_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Shader> shader;
            ShaderVar mModelShaderVar, mNormalShaderVar;

            std::vector<std::unique_ptr<GenericRenderer>> meshRenderers;
    };

}

#endif
