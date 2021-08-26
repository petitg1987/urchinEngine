#pragma once

#include <memory>

#include <graphic/texture/Texture.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/shader/model/Shader.h>

namespace urchin {

    class BloomEffectApplier {
        public:
            enum FetchTextureQuality {
                FETCH_4,
                FETCH_13
            };

            struct Config {
                unsigned int maximumIterations = 15;
                FetchTextureQuality fetchTextureQuality = FETCH_13;
            };

            BloomEffectApplier();
            ~BloomEffectApplier();

            void onTextureUpdate(const std::shared_ptr<Texture>&, std::optional<RenderTarget*>);
            const std::shared_ptr<Texture>& getBloomedTexture() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void applyBloom();

        private:
            void checkConfig() const;
            void clearRendering();
            void refreshRenderer();

            Config config;
            unsigned int sceneWidth, sceneHeight;
            std::shared_ptr<Texture> lightingPassTexture;

            RenderTarget* finalRenderTarget;
            std::vector<std::shared_ptr<Texture>> intermediateTextures;
            std::shared_ptr<Texture> bloomedTexture;
            std::unique_ptr<OffscreenRender> offscreenBloomRenderTarget;

            //pre-filter + blur
            std::unique_ptr<OffscreenRender> bloomFilteredRenderTarget;
            std::unique_ptr<Shader> bloomPreFilterShader;
            struct {
                alignas(4) float threshold;
            } bloomTweak;
            std::unique_ptr<GenericRenderer> preFilterRenderer;

            //down sample
            std::unique_ptr<OffscreenRender> bloomDownSampleRenderTarget;
            std::unique_ptr<Shader> bloomDownSampleShader;
            std::unique_ptr<GenericRenderer> downSampleRenderer;

            //up sample
            std::unique_ptr<OffscreenRender> bloomUpSampleRenderTarget;
            std::unique_ptr<Shader> bloomUpSampleShader;
            std::unique_ptr<GenericRenderer> upSampleRenderer;

            //combine
            std::unique_ptr<Shader> bloomCombineShader;
            std::unique_ptr<GenericRenderer> combineRenderer;
    };

}