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

            void refreshInputTexture(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateQuality(AntiAliasingQuality);

            void applyAntiAliasing(uint32_t, unsigned int);

        private:
            int getOutputTextureIndex() const;
            int getHistoryTextureIndex() const;

            void createOrUpdateRenderData();
            void createOrUpdateVelocityRenderData();
            void createOrUpdateResolveRenderData();
            void freeRenderData();
            void freeVelocityRenderData();
            void freeResolveRenderData();

            void createOrUpdateVelocityRenderer();
            void createOrUpdateResolveRenderer();

            void generateVelocityTexture(uint32_t) const;

            static constexpr uint32_t DEPTH_TEX_UNIFORM_BINDING = 0;

            static constexpr uint32_t SCENE_TEX_UNIFORM_BINDING = 0;
            static constexpr uint32_t VELOCITY_TEX_UNIFORM_BINDING = 1;
            static constexpr uint32_t HISTORY_TEX_UNIFORM_BINDING = 2;

            //properties
            bool isTestMode;
            bool isEnabled;
            AntiAliasingQuality quality;

            //data
            std::shared_ptr<Texture> depthTexture;
            std::shared_ptr<Texture> sceneTexture;
            bool copySceneTexToHistory;
            unsigned int frameCount;

            //display - velocity
            std::shared_ptr<Texture> velocityTexture;
            std::unique_ptr<OffscreenRender> velocityRenderTarget;
            std::unique_ptr<Shader> taaVelocityShader;
            std::unique_ptr<GenericRenderer> velocityRenderer;

            //display - resolve
            std::array<std::shared_ptr<Texture>, 2> outputOrHistoryTextures;
            std::unique_ptr<OffscreenRender> resolveRenderTarget;
            std::unique_ptr<Shader> taaResolveShader;
            std::unique_ptr<GenericRenderer> resolveRenderer;
    };

}
