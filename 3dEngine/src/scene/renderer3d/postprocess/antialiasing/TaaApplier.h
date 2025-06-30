#pragma once

#include <memory>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingQuality.h>
#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class TaaApplier {
        public:
            explicit TaaApplier(bool);
            ~TaaApplier();

            void enableApplier(bool);

            void applyCameraJitter(Camera& camera);

            void refreshInputTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateQuality(AntiAliasingQuality);

            void applyAntiAliasing(uint32_t, unsigned int);

        private:
            int getOutputTextureIndex() const;
            int getHistoryTextureIndex() const;

            void createOrUpdateRenderData();
            void freeRenderData();

            void createOrUpdateRenderer();
            void createOrUpdateFxaaShader();

            static constexpr uint32_t INPUT_TEX_UNIFORM_BINDING = 0;
            static constexpr uint32_t HISTORY_TEX_UNIFORM_BINDING = 1;

            //properties
            bool isTestMode;
            bool isEnabled;
            AntiAliasingQuality quality;

            //data
            bool copyInputTexToHistory;
            unsigned int frameCount;

            //display
            std::unique_ptr<OffscreenRender> resolveRenderTarget;
            std::shared_ptr<Texture> inputTexture;
            std::array<std::shared_ptr<Texture>, 2> outputOrHistoryTextures;
            std::unique_ptr<Shader> taaResolveShader;
            std::unique_ptr<GenericRenderer> resolveRenderer;
    };

}
