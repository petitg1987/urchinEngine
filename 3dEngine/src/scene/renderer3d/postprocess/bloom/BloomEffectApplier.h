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

            BloomEffectApplier(const Config&, bool);
            ~BloomEffectApplier();

            void refreshInputTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void applyBloom(uint32_t, unsigned int) const;

        private:
            void checkConfig() const;
            void refreshRenderers();
            void clearRenderers();

            static constexpr float UP_SAMPLE_SCALE = 0.95f;

            static constexpr uint32_t PF_TWEAK_UNIFORM_BINDING = 0;
            static constexpr uint32_t PF_TEXEL_SIZE_UNIFORM_BINDING = 1;
            static constexpr uint32_t PF_INPUT_TEX_UNIFORM_BINDING = 2;
            static constexpr uint32_t PF_OUTPUT_TEX_UNIFORM_BINDING = 3;

            static constexpr uint32_t DS_TEXEL_SIZE_UNIFORM_BINDING = 0;
            static constexpr uint32_t DS_INPUT_TEX_UNIFORM_BINDING = 1;
            static constexpr uint32_t DS_OUTPUT_TEX_UNIFORM_BINDING = 2;

            static constexpr uint32_t US_TEXEL_SIZE_UNIFORM_BINDING = 0;
            static constexpr uint32_t US_INPUT_TEX_UNIFORM_BINDING = 1;

            static constexpr uint32_t CR_TEXEL_SIZE_UNIFORM_BINDING = 0;
            static constexpr uint32_t CR_BLOOM_STEP_TEX_UNIFORM_BINDING = 1;
            static constexpr uint32_t CR_HDR_TEX_UNIFORM_BINDING = 2;

            struct BloomShadersConst {
                uint32_t qualityTextureFetch;
                float upSampleScale;
            };

            Config config;
            bool isTestMode;
            unsigned int sceneWidth;
            unsigned int sceneHeight;
            std::shared_ptr<Texture> inputHdrTexture;
            std::vector<std::shared_ptr<Texture>> bloomStepTextures;
            std::shared_ptr<Texture> bloomCombineTexture;

            //pre-filter
            std::unique_ptr<RenderTarget> preFilterRenderTarget;
            std::unique_ptr<GenericCompute> preFilterCompute;
            std::unique_ptr<Shader> preFilterShader;
            struct PreFilterTweak {
                alignas(16) Vector3<float> softCurveParams;
                alignas(4) float threshold;
            } preFilterTweak;

            //down sample
            std::unique_ptr<Shader> downSampleShader;
            std::vector<std::unique_ptr<RenderTarget>> downSampleRenderTargets;
            std::vector<std::unique_ptr<GenericCompute>> downSampleComputes;

            //up sample
            std::unique_ptr<Shader> upSampleShader;
            std::vector<std::unique_ptr<RenderTarget>> upSampleRenderTargets;
            std::vector<std::unique_ptr<GenericRenderer>> upSampleRenderers;

            //combine
            std::unique_ptr<Shader> combineShader;
            std::unique_ptr<OffscreenRender> combineRenderTarget;
            std::unique_ptr<GenericRenderer> combineRenderer;
    };

}