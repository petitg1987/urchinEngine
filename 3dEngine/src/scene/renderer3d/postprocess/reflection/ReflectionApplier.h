#pragma once

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class ReflectionApplier {
        public:
            explicit ReflectionApplier(bool);
            ~ReflectionApplier();

            void onCameraProjectionUpdate(const Camera&);

            void refreshInputTexture(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void applyReflection(std::uint32_t, unsigned int) const;

        private:
            void createOrUpdateRenderingObjects();
            void clearRenderingObjects();
            void createOrUpdateRenderTargets();
            void createOrUpdateRenderers();
            void createOrUpdateShaders();

            static constexpr uint32_t PROJECTION_UNIFORM_BINDING = 0;
            static constexpr uint32_t DEPTH_TEX_UNIFORM_BINDING = 1;
            static constexpr uint32_t NORMAL_AMBIENT_TEX_UNIFORM_BINDING = 2;
            static constexpr uint32_t MATERIAL_TEX_UNIFORM_BINDING = 3;
            static constexpr uint32_t R_COLOR_ILLUMINATED_TEX_UNIFORM_BINDING = 4;

            static constexpr uint32_t REFLECTION_COLOR_TEX_UNIFORM_BINDING = 0;
            static constexpr uint32_t R_COMBINE_ILLUMINATED_TEX_UNIFORM_BINDING = 1;

            bool useNullRenderTarget;
            std::shared_ptr<Texture> depthTexture;
            std::shared_ptr<Texture> normalAndAmbientTexture;
            std::shared_ptr<Texture> materialTexture;
            std::shared_ptr<Texture> illuminatedTexture;

            struct {
                alignas(16) Matrix4<float> projectionMatrix;
                alignas(16) Matrix4<float> inverseProjectionMatrix;
            } projection;
            std::shared_ptr<Texture> reflectionColorOutputTexture;
            std::unique_ptr<RenderTarget> reflectionColorRenderTarget;
            std::unique_ptr<Shader> reflectionColorShader;
            std::unique_ptr<GenericRenderer> reflectionColorRenderer;

            std::shared_ptr<Texture> reflectionCombineOutputTexture;
            std::unique_ptr<RenderTarget> reflectionCombineRenderTarget;
            std::unique_ptr<Shader> reflectionCombineShader;
            std::unique_ptr<GenericRenderer> reflectionCombineRenderer;
    };

}
