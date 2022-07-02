#pragma once

#include <UrchinCommon.h>

#include <api/render/shader/model/Shader.h>
#include <scene/renderer3d/camera/Camera.h>
#include <api/render/GenericRenderer.h>
#include <texture/filter/bilateralblur/BilateralBlurFilter.h>

namespace urchin {

    class AmbientOcclusionManager {
        public:
            enum AOTextureSize {
                FULL_SIZE = 0,
                HALF_SIZE = 1
            };
            enum AOTextureBits {
                AO_8_BITS,
                AO_16_BITS
            };

            struct Config {
                AOTextureSize textureSize = AOTextureSize::HALF_SIZE;
                AOTextureBits textureBits = AOTextureBits::AO_8_BITS;
                unsigned int kernelSamples = 32;
                float radius = 0.3f; //scope radius in units
                float ambientOcclusionStrength = 0.12f;
                float depthStartAttenuation = 0.995f;
                float depthEndAttenuation = 0.997f;
                unsigned int noiseTextureSize = 4;
                float bias = 0.05f;
                bool isBlurActivated = true;
                unsigned int blurSize = 7;
                float blurSharpness = 40.0f;
            };

            AmbientOcclusionManager(const Config&, bool);
            ~AmbientOcclusionManager();

            void onTextureUpdate(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);
            void onCameraProjectionUpdate(const Camera&);

            void updateConfig(const Config&);
            const Config& getConfig() const;

            const std::shared_ptr<Texture>& getAmbientOcclusionTexture() const;

            void updateAOTexture(std::uint32_t, unsigned int, const Camera&);
            void loadAOTexture(GenericRenderer&, std::size_t) const;

        private:
            struct AmbientOcclusionShaderConst {
                uint32_t kernelSamples;
                float radius;
                float ambientOcclusionStrength;
                float depthStartAttenuation;
                float depthEndAttenuation;
                uint32_t noiseTextureSize;
                float bias;
            };

            void checkConfig() const;

            void createOrUpdateAO();
            void createOrUpdateAOTexture();
            void createOrUpdateRenderer();
            void createOrUpdateAOShader();
            void generateKernelSamples();
            void generateNoiseTexture();
            void exportSVG(std::string, const std::vector<Vector4<float>>&) const;

            int retrieveTextureSizeFactor() const;

            static constexpr unsigned int KERNEL_SAMPLES_SHADER_LIMIT = 64; //must be equals to 'KERNEL_SAMPLES' in AO shader

            //general data
            Config config;
            bool useNullRenderTarget;
            float nearPlane;
            float farPlane;
            unsigned int textureSizeX;
            unsigned int textureSizeY;

            //frame buffer object
            std::unique_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Texture> ambientOcclusionTexture;

            //ambient occlusion shader
            std::unique_ptr<Shader> ambientOcclusionShader;
            std::shared_ptr<Texture> noiseTexture;
            struct {
                alignas(16) Matrix4<float> inverseProjectionMatrix;
                alignas(16) Matrix4<float> projectionMatrix;
            } projection;
            struct {
                alignas(16) Matrix4<float> inverseProjectionViewMatrix;
                alignas(16) Matrix4<float> viewMatrix;
            } positioningData;
            std::vector<Vector4<float>> ssaoKernel;
            Vector2<float> resolution;

            //visual data
            std::shared_ptr<Texture> depthTexture;
            std::shared_ptr<Texture> normalAndAmbientTexture;
            std::unique_ptr<GenericRenderer> renderer;

            std::unique_ptr<BilateralBlurFilter> verticalBlurFilter;
            std::unique_ptr<BilateralBlurFilter> horizontalBlurFilter;
    };

}
