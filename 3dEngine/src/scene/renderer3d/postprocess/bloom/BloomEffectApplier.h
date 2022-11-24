#pragma once

#include <memory>

#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class BloomEffectApplier {
        public:
            enum TextureFetchQuality {
                BEST_QUALITY = 0,
                MEDIUM_QUALITY,
                LOW_QUALITY
            };

            struct Config {
                unsigned int maxIterations = 15;
                TextureFetchQuality textureFetchQuality = BEST_QUALITY;
            };

            BloomEffectApplier(const Config&, RenderTarget&, float);
            ~BloomEffectApplier();

            void onTextureUpdate(const std::shared_ptr<Texture>&);
            void onGammaFactorUpdate(float);

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void applyBloom(std::uint32_t, unsigned int) const;

        private:
            void checkConfig() const;
            void refreshRenderers();
            void clearRenderers();

            std::unique_ptr<Shader> createShader(const std::string&, const std::string&, std::unique_ptr<ShaderConstants>);

            static constexpr float UP_SAMPLE_SCALE = 0.95f;

            struct BloomShadersConst {
                uint32_t qualityTextureFetch;
                float upSampleScale;
            };

            Config config;
            RenderTarget& outputRenderTarget;
            unsigned int sceneWidth;
            unsigned int sceneHeight;
            std::shared_ptr<Texture> inputHdrTexture;
            std::vector<std::shared_ptr<Texture>> bloomStepTextures;

            //pre-filter
            std::unique_ptr<RenderTarget> preFilterRenderTarget;
            std::unique_ptr<GenericRenderer> preFilterRenderer;
            std::unique_ptr<Shader> preFilterShader;
            struct {
                alignas(16) Vector3<float> softCurveParams;
                alignas(4) float threshold;
            } preFilterTweak;

            //down sample
            std::unique_ptr<Shader> downSampleShader;
            std::vector<std::unique_ptr<RenderTarget>> downSampleRenderTargets;
            std::vector<std::unique_ptr<GenericRenderer>> downSampleRenderers;

            //up sample
            std::unique_ptr<Shader> upSampleShader;
            std::vector<std::unique_ptr<RenderTarget>> upSampleRenderTargets;
            std::vector<std::unique_ptr<GenericRenderer>> upSampleRenderers;

            //combine
            std::unique_ptr<Shader> combineShader;
            std::unique_ptr<GenericRenderer> combineRenderer;
            float gammaFactor;
    };

}