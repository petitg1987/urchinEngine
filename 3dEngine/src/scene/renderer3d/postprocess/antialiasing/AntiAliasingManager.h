#pragma once

#include <memory>

#include <graphic/render/shader/model/Shader.h>
#include <graphic/render/GenericRenderer.h>
#include <graphic/render/target/OffscreenRender.h>

namespace urchin {

    class AntiAliasingManager {
        public:
            enum Quality {
                LOW = 0,
                MEDIUM,
                HIGH
            };

            struct Config {
                Quality quality = AntiAliasingManager::Quality::HIGH;
            };

            explicit AntiAliasingManager();

            void onTextureUpdate(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

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

            void createOrUpdateRendering();
            void createOrUpdateFxaaShader();

            //properties
            Config config;

            //display
            std::unique_ptr<OffscreenRender> renderTarget;
            std::shared_ptr<Texture> inputTexture, outputTexture;
            std::unique_ptr<Shader> fxaaShader;
            Point2<float> invSceneSize;
            std::unique_ptr<GenericRenderer> renderer;
    };

}
