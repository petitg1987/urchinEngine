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
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void applyBloom();

        private:
            void checkConfig() const;
            void clearOutputRenderTarget();
            void refreshRenderers();
            void clearRenderers();

            Config config;
            unsigned int sceneWidth, sceneHeight;
            std::shared_ptr<Texture> inputHdrTexture;
            std::vector<std::shared_ptr<Texture>> bloomStepTextures;

            //pre-filter
            std::unique_ptr<OffscreenRender> preFilterRenderTarget;
            std::unique_ptr<GenericRenderer> preFilterRenderer;
            std::unique_ptr<Shader> preFilterShader;
            struct {
                alignas(4) float threshold;
            } preFilterTweak;

            //down sample
            std::unique_ptr<Shader> downSampleShader;
            std::vector<std::unique_ptr<OffscreenRender>> downSampleRenderTargets;
            std::vector<std::unique_ptr<GenericRenderer>> downSampleRenderers;

            //up sample
            std::unique_ptr<Shader> upSampleShader;
            std::vector<std::unique_ptr<OffscreenRender>> upSampleRenderTargets;
            std::vector<std::unique_ptr<GenericRenderer>> upSampleRenderers;

            //combine
            std::optional<std::shared_ptr<Texture>> outputLdrTexture;
            std::optional<std::unique_ptr<OffscreenRender>> outputOffscreenRenderTarget;
            RenderTarget* outputRenderTarget; //either outputOffscreenRenderTarget or a custom render target
            std::unique_ptr<Shader> combineShader;
            std::unique_ptr<GenericRenderer> combineRenderer;
    };

}