#pragma once

#include <memory>

#include <api/texture/Texture.h>
#include <api/render/target/RenderTarget.h>
#include <api/render/shader/model/Shader.h>

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

            explicit BloomEffectApplier(RenderTarget&);
            ~BloomEffectApplier();

            void onTextureUpdate(const std::shared_ptr<Texture>&);

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void applyBloom(unsigned int);

        private:
            struct BloomShadersConst {
                uint32_t qualityTextureFetch;
                float upSampleScale;
            };

            void checkConfig() const;
            void refreshRenderers();
            void clearRenderers();

            std::unique_ptr<Shader> createShader(const std::string&, const std::string&, std::unique_ptr<ShaderConstants>);

            RenderTarget& outputRenderTarget;
            Config config;
            unsigned int sceneWidth, sceneHeight;
            std::shared_ptr<Texture> inputHdrTexture;
            std::vector<std::shared_ptr<Texture>> bloomStepTextures;

            //pre-filter
            std::unique_ptr<RenderTarget> preFilterRenderTarget;
            std::unique_ptr<GenericRenderer> preFilterRenderer;
            std::unique_ptr<Shader> preFilterShader;
            struct {
                alignas(4) float threshold;
                alignas(16) Vector3<float> softCurveParams;
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
            float exposureFactor;
    };

}