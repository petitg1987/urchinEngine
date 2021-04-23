#ifndef URCHINENGINE_ANTIALIASINGMANAGER_H
#define URCHINENGINE_ANTIALIASINGMANAGER_H

#include <memory>

#include "graphic/render/shader/model/Shader.h"
#include "graphic/render/GenericRenderer.h"
#include "graphic/render/target/RenderTarget.h"

namespace urchin {

    class AntiAliasingManager {
        public:
            enum Quality {
                LOW = 0,
                MEDIUM,
                HIGH
            };

            explicit AntiAliasingManager(std::shared_ptr<RenderTarget>);

            void onTextureUpdate(const std::shared_ptr<Texture>&);
            void onResize(unsigned int, unsigned int);

            void setQuality(Quality quality);

            void applyAntiAliasing();

        private:
            struct AntiAliasingShaderConst {
                uint32_t qualityPs;
                float qualityP0;
                float qualityP1;
                float qualityP2;
                float qualityP3;
                float qualityP4;
                float qualityP5;
                float qualityP6;
                float qualityP7;
                float qualityP8;
                float qualityP9;
                float qualityP10;
                float qualityP11;
            };

            void loadFxaaShader();

            //properties
            std::shared_ptr<RenderTarget> renderTarget;
            Quality quality;

            //display
            std::shared_ptr<Shader> fxaaShader;
            Point2<float> invSceneSize;
            std::shared_ptr<GenericRenderer> renderer;
    };

}

#endif
