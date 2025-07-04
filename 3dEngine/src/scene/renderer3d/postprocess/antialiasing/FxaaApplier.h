#pragma once

#include <memory>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingQuality.h>

namespace urchin {

    class FxaaApplier {
        public:
            FxaaApplier(bool, bool, AntiAliasingQuality);
            ~FxaaApplier();

            void enableApplier(bool);

            void refreshInputTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateQuality(AntiAliasingQuality);

            void applyAntiAliasing(uint32_t, unsigned int) const;

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

            void createOrUpdateRenderData();
            void freeRenderData();

            void createOrUpdateRenderer();
            void createOrUpdateFxaaShader();

            static constexpr uint32_t INV_SCENE_SIZE_UNIFORM_BINDING = 0;
            static constexpr uint32_t INPUT_TEX_UNIFORM_BINDING = 1;

            //properties
            bool isTestMode;
            bool isEnabled;
            AntiAliasingQuality quality;

            //display
            std::unique_ptr<OffscreenRender> renderTarget;
            std::shared_ptr<Texture> inputTexture;
            std::shared_ptr<Texture> outputTexture;
            std::unique_ptr<Shader> fxaaShader;
            std::unique_ptr<GenericRenderer> renderer;
    };

}
