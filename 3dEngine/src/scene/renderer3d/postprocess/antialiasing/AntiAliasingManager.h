#ifndef URCHINENGINE_ANTIALIASINGMANAGER_H
#define URCHINENGINE_ANTIALIASINGMANAGER_H

#include <memory>

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/GenericRenderer.h"
#include "graphic/render/target/RenderTarget.h"

namespace urchin {

    class AntiAliasingManager {
        public:
            enum Quality {
                LOW = 0,
                MEDIUM,
                HIGH,
                VERY_HIGH
            };

            explicit AntiAliasingManager(std::shared_ptr<RenderTarget>);

            void onTextureUpdate(const std::shared_ptr<Texture>&);
            void onResize(unsigned int, unsigned int);

            void setQuality(Quality quality);

            void applyAntiAliasing();

        private:
            void loadFxaaShader();

            //properties
            std::shared_ptr<RenderTarget> renderTarget;
            Quality quality;

            //display
            std::shared_ptr<Shader> fxaaShader;
            ShaderVar invSceneSizeShaderVar;
            Point2<float> invSceneSize;
            std::unique_ptr<GenericRenderer> renderer;
    };

}

#endif
