#pragma once

#include <memory>
#include <array>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingQuality.h>
#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class TaaApplier {
        public:
            TaaApplier(bool, bool, AntiAliasingQuality);
            ~TaaApplier();

            void enableApplier(bool);

            void applyJitter(Camera&, unsigned int, unsigned int);
            Vector2<float> getCurrentJitter() const;
            Vector2<float> getPreviousJitter() const;

            void refreshInputTexture(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateQuality(AntiAliasingQuality);

            void applyAntiAliasing(uint32_t, unsigned int, const Camera&);

        private:
            static constexpr std::array HALTON_SEQUENCE_X = {0.500000f, 0.250000f, 0.750000f, 0.125000f, 0.625000f, 0.375000f, 0.875000f, 0.062500f,
                0.562500f, 0.312500f, 0.812500f, 0.187500f, 0.687500f, 0.437500f, 0.937500f, 0.031250f};
            static constexpr std::array HALTON_SEQUENCE_Y = {0.333333f, 0.666667f, 0.111111f, 0.444444f, 0.777778f, 0.222222f, 0.555556f, 0.888889f,
                0.037037f, 0.370370f, 0.703704f, 0.148148f, 0.481481f, 0.814815f, 0.259259f, 0.592593f};

            struct AntiAliasingShaderConst {
                uint32_t highQuality;
            };

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
            Vector2<float> currentJitter;
            Vector2<float> previousJitter;
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
