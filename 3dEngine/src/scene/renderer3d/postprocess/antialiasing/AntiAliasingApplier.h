#pragma once

#include <memory>

#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class AntiAliasingApplier {
        public:
            enum Quality {
                LOW = 0,
                MEDIUM,
                HIGH
            };

            struct Config {
                Quality quality = AntiAliasingApplier::Quality::HIGH;
            };

            AntiAliasingApplier(const Config&, bool);
            ~AntiAliasingApplier();

            void refreshInputTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void applyAntiAliasing(std::uint32_t, unsigned int) const;

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

            void clearRenderer();
            void createOrUpdateRenderer();
            void createOrUpdateFxaaShader();

            static constexpr uint32_t INV_SCENE_SIZE_UNIFORM_BINDING = 0;
            static constexpr uint32_t INPUT_TEX_UNIFORM_BINDING = 1;

            //properties
            bool useNullRenderTarget;
            Config config;

            //display
            std::unique_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Texture> inputTexture;
            std::shared_ptr<Texture> outputTexture;
            std::unique_ptr<Shader> fxaaShader;
            Point2<float> invSceneSize;
            std::unique_ptr<GenericRenderer> renderer;
    };

}
