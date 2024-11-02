#pragma once

#include <UrchinCommon.h>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/camera/Camera.h>
#include <texture/filter/gaussianblur/GaussianBlurFilter.h>

namespace urchin {

    class AmbientOcclusionManager final : public Observable {
        public:
            enum NotificationType {
                AMBIENT_OCCLUSION_STRENGTH_UPDATE
            };
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
                float radius = 0.05f; //scope radius in units
                float ambientOcclusionStrength = 0.08f;
                float distanceStartAttenuation = 12.0f; //distance in units
                float distanceEndAttenuation = 14.0f; //distance in units
                unsigned int noiseTextureSize = 64;
                float biasMultiplier = 0.002f;
                float biasDistanceMultiplier = 1.9f;
                bool isBlurActivated = true;
                unsigned int blurRadius = 3;
                float maxBlurDistance = 10.0f;
            };

            AmbientOcclusionManager(const Config&, bool);
            ~AmbientOcclusionManager() override;

            void onCameraProjectionUpdate(const Camera&);

            void refreshInputTextures(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);

            float getAmbientOcclusionStrength() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void updateAOTexture(uint32_t, unsigned int, const Camera&);

            const std::shared_ptr<Texture>& getAmbientOcclusionTexture() const;
            void loadAOTexture(GenericRenderer&, uint32_t) const;

        private:
            struct AmbientOcclusionShaderConst {
                uint32_t kernelSamples;
                float radius;
                float depthStartAttenuation;
                float depthEndAttenuation;
                float biasMultiplier;
                float biasDistanceMultiplier;
            };

            void checkConfig() const;

            void createOrUpdateRenderingObjects();
            void createOrUpdateRenderTarget();
            void createOrUpdateRenderer();
            void createOrUpdateAOShader();
            void generateKernelSamples();
            void generateNoiseTexture();
            void exportSVG(std::string, const std::vector<Vector4<float>>&) const;

            int retrieveTextureSizeFactor() const;

            static constexpr unsigned int KERNEL_SAMPLES_SHADER_LIMIT = 64; //must be equals to 'KERNEL_SAMPLES' in AO shader

            static constexpr uint32_t PROJECTION_UNIFORM_BINDING = 0;
            static constexpr uint32_t POSITIONING_DATA_UNIFORM_BINDING = 1;
            static constexpr uint32_t SSAO_KERNEL_UNIFORM_BINDING = 2;
            static constexpr uint32_t DEPTH_TEX_UNIFORM_BINDING = 3;
            static constexpr uint32_t NORMAL_AMBIENT_TEX_UNIFORM_BINDING = 4;
            static constexpr uint32_t NOISE_TEX_UNIFORM_BINDING = 5;
            static constexpr uint32_t AO_TEX_UNIFORM_BINDING = 6;

            //general data
            Config config;
            bool isTestMode;
            float nearPlane;
            float farPlane;

            //frame buffer object
            std::unique_ptr<OffscreenRender> renderTarget;
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
            Vector2<float> sceneResolution;

            //visual data
            std::shared_ptr<Texture> depthTexture;
            std::shared_ptr<Texture> normalAndAmbientTexture;
            std::unique_ptr<GenericCompute> compute;

            std::unique_ptr<GaussianBlurFilter> verticalBlurFilter;
            std::unique_ptr<GaussianBlurFilter> horizontalBlurFilter;
    };

}
