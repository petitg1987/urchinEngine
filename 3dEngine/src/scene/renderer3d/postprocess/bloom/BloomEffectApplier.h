#pragma once

#include <memory>

#include <graphic/texture/Texture.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/shader/model/Shader.h>

namespace urchin {

    class BloomEffectApplier {
        public:
            enum TextureFetchQuality {
                QUALITY_FETCH,
                PERFORMANCE_FETCH
            };

            struct Config {
                unsigned int maxIterations = 15;
                TextureFetchQuality textureFetchQuality = QUALITY_FETCH;
            };

            explicit BloomEffectApplier(RenderTarget&);
            ~BloomEffectApplier();

            void onTextureUpdate(const std::shared_ptr<Texture>&);

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void applyBloom();

        private:
            struct BloomShadersConst {
                bool qualityTextureFetch;
                float upSampleScale;
            };

            void checkConfig() const;
            void refreshRenderers();
            void clearRenderers();

            RenderTarget& outputRenderTarget;
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
                alignas(16) Vector3<float> softCurveParams;
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
            std::unique_ptr<Shader> combineShader;
            std::unique_ptr<GenericRenderer> combineRenderer;
            float exposureFactor;
    };

}