#pragma once

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/camera/Camera.h>
#include <texture/filter/gaussianblur/GaussianBlurFilter.h>

namespace urchin {

    class ReflectionApplier {
        public:
            enum ReflectionTextureSize {
                FULL_SIZE = 0,
                HALF_SIZE = 1
            };

            struct Config {
                ReflectionTextureSize textureSize = ReflectionTextureSize::HALF_SIZE;
                float maxDistance = 1.75f;
                float hitThreshold = 0.075f;
                float skipPixelCountOnFirstPass = 3.0f;
                unsigned int numStepsOnSecondPass = 8;
                float reflectionStrength = 0.4f;
                unsigned int blurSize = 5;
                float blurSharpness = 40.0f;
            };

            ReflectionApplier(const Config&, bool);
            ~ReflectionApplier();

            void onCameraProjectionUpdate(const Camera&);

            void refreshInputTexture(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void applyReflection(std::uint32_t, unsigned int, const Camera&);

        private:
            struct ReflectionColorShaderConst {
                float maxDistance;
                float hitThreshold;
                float skipPixelCountOnFirstPass;
                uint32_t numStepsOnSecondPass;
            };
            struct ReflectionCombineShaderConst {
                float reflectionStrength;
            };

            void createOrUpdateRenderingObjects();
            void clearRenderingObjects();
            void createOrUpdateRenderTargets();
            void createOrUpdateRenderers();
            void createOrUpdateShaders();

            int retrieveTextureSizeFactor() const;

            static constexpr uint32_t PROJECTION_DATA_UNIFORM_BINDING = 0;
            static constexpr uint32_t POSITIONING_DATA_UNIFORM_BINDING = 1;
            static constexpr uint32_t DEPTH_TEX_UNIFORM_BINDING = 2;
            static constexpr uint32_t NORMAL_AMBIENT_TEX_UNIFORM_BINDING = 3;
            static constexpr uint32_t MATERIAL_TEX_UNIFORM_BINDING = 4;
            static constexpr uint32_t R_COLOR_ILLUMINATED_TEX_UNIFORM_BINDING = 5;

            static constexpr uint32_t R_COMBINE_ILLUMINATED_TEX_UNIFORM_BINDING = 0;
            static constexpr uint32_t REFLECTION_COLOR_TEX_UNIFORM_BINDING = 1;

            Config config;
            bool useNullRenderTarget;
            float nearPlane;
            float farPlane;

            std::shared_ptr<Texture> depthTexture;
            std::shared_ptr<Texture> normalAndAmbientTexture;
            std::shared_ptr<Texture> materialTexture;
            std::shared_ptr<Texture> illuminatedTexture;

            struct {
                alignas(16) Matrix4<float> projectionMatrix;
                alignas(16) Matrix4<float> inverseProjectionMatrix;
            } projectionData;
            struct {
                alignas(16) Matrix4<float> viewMatrix;
            } positioningData;
            std::shared_ptr<Texture> reflectionColorTexture;
            std::unique_ptr<RenderTarget> reflectionColorRenderTarget;
            std::unique_ptr<Shader> reflectionColorShader;
            std::unique_ptr<GenericRenderer> reflectionColorRenderer;

            std::unique_ptr<GaussianBlurFilter> verticalBlurFilter;
            std::unique_ptr<GaussianBlurFilter> horizontalBlurFilter;

            std::shared_ptr<Texture> reflectionCombineTexture;
            std::unique_ptr<RenderTarget> reflectionCombineRenderTarget;
            std::unique_ptr<Shader> reflectionCombineShader;
            std::unique_ptr<GenericRenderer> reflectionCombineRenderer;
    };

}
