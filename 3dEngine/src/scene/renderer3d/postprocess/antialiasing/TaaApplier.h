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

            void updateCamera(Camera&, unsigned int, unsigned int);

            void refreshInputTexture(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateQuality(AntiAliasingQuality);

            void applyAntiAliasing(uint32_t, unsigned int, const Camera&);

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

            void generateVelocityTexture(uint32_t, const Camera&);

            static constexpr uint32_t VELOCITY_POSITIONING_DATA_UNIFORM_BINDING = 0;
            static constexpr uint32_t VELOCITY_DEPTH_TEX_UNIFORM_BINDING = 1;

            static constexpr uint32_t RESOLVE_SCENE_TEX_UNIFORM_BINDING = 0;
            static constexpr uint32_t RESOLVE_DEPTH_TEX_UNIFORM_BINDING = 1;
            static constexpr uint32_t RESOLVE_VELOCITY_TEX_UNIFORM_BINDING = 2;
            static constexpr uint32_t RESOLVE_HISTORY_TEX_UNIFORM_BINDING = 3;

            //properties
            bool isTestMode;
            bool isEnabled;
            AntiAliasingQuality quality;

            //data
            std::shared_ptr<Texture> depthTexture;
            std::shared_ptr<Texture> sceneTexture;
            bool copySceneTexToHistory;
            unsigned int frameCount;
            struct PositioningData {
                alignas(16) Matrix4<float> inverseProjectionViewMatrix;
                alignas(16) Matrix4<float> previousProjectionViewMatrix;
            } positioningData;

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
