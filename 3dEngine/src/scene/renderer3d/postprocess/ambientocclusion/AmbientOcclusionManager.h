#ifndef URCHINENGINE_AMBIENTOCCLUSIONMANAGER_H
#define URCHINENGINE_AMBIENTOCCLUSIONMANAGER_H

#include <UrchinCommon.h>

#include <graphic/render/shader/model/Shader.h>
#include <scene/renderer3d/camera/Camera.h>
#include <graphic/render/GenericRenderer.h>
#include <texture/filter/bilateralblur/BilateralBlurFilter.h>

#define DEFAULT_TEXTURE_SIZE AOTextureSize::HALF_SIZE
#define DEFAULT_KERNEL_SAMPLES 32
#define DEFAULT_RADIUS 0.35f
#define DEFAULT_AO_STRENGTH 0.10f
#define DEFAULT_DEPTH_START_ATTENUATION 0.995f
#define DEFAULT_DEPTH_END_ATTENUATION 0.997f
#define DEFAULT_NOISE_TEXTURE_SIZE 4
#define DEFAULT_BIAS 0.15f
#define DEFAULT_BLUR_SIZE 7
#define DEFAULT_BLUR_SHARPNESS 40.0f

namespace urchin {

    class AmbientOcclusionManager {
        public:
            enum AOTextureSize {
                FULL_SIZE = 0,
                HALF_SIZE = 1
            };

            struct Config {
                AOTextureSize textureSize = DEFAULT_TEXTURE_SIZE;
                unsigned int kernelSamples = DEFAULT_KERNEL_SAMPLES;
                float radius = DEFAULT_RADIUS; //scope radius in units
                float ambientOcclusionStrength = DEFAULT_AO_STRENGTH;
                float depthStartAttenuation = DEFAULT_DEPTH_START_ATTENUATION;
                float depthEndAttenuation = DEFAULT_DEPTH_END_ATTENUATION;
                unsigned int noiseTextureSize = DEFAULT_NOISE_TEXTURE_SIZE;
                float bias = DEFAULT_BIAS;
                bool isBlurActivated = true;
                unsigned int blurSize = DEFAULT_BLUR_SIZE;
                float blurSharpness = DEFAULT_BLUR_SHARPNESS;
            };

            AmbientOcclusionManager();
            ~AmbientOcclusionManager();

            void onTexturesUpdate(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);
            void onResize(unsigned int, unsigned int);
            void onCameraProjectionUpdate(const Camera*);

            void updateConfig(const Config&);
            const Config& getConfig() const;

            const std::shared_ptr<Texture>& getAmbientOcclusionTexture() const;

            void updateAOTexture(const Camera*);
            void loadAOTexture(const std::shared_ptr<GenericRenderer>&, std::size_t) const;

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
            void createOrUpdateAOShader();
            void createOrUpdateAOTexture();
            void createOrUpdateRenderer();
            void generateKernelSamples();
            void generateNoiseTexture();
            void exportSVG(const std::string&, const std::vector<Vector4<float>>&) const;

            int retrieveTextureSizeFactor() const;

            static const unsigned int KERNEL_SAMPLES_SHADER_LIMIT;

            //scene information
            float nearPlane, farPlane;

            //config
            Config config;
            unsigned int textureSizeX, textureSizeY;

            //frame buffer object
            std::shared_ptr<OffscreenRender> offscreenRenderTarget;
            std::shared_ptr<Texture> ambientOcclusionTexture;

            //ambient occlusion shader
            std::shared_ptr<Shader> ambientOcclusionShader;
            std::shared_ptr<Texture> noiseTexture;
            struct {
                alignas(16) Matrix4<float> inverseProjectionViewMatrix;
                alignas(16) Matrix4<float> projectionMatrix;
                alignas(16) Matrix4<float> viewMatrix;
            } positioningData;
            std::vector<Vector4<float>> ssaoKernel;
            Vector2<float> resolution;

            //visual data
            std::shared_ptr<Texture> depthTexture;
            std::shared_ptr<Texture> normalAndAmbientTexture;
            std::shared_ptr<GenericRenderer> renderer;

            std::unique_ptr<BilateralBlurFilter> verticalBlurFilter;
            std::unique_ptr<BilateralBlurFilter> horizontalBlurFilter;
    };

}

#endif
