#pragma once

#include <string>

#include <texture/filter/TextureFilter.h>

namespace urchin {

    class GaussianBlurFilterBuilder;

    class GaussianBlurFilter final : public TextureFilter {
        public:
            enum BlurDirection {
                VERTICAL,
                HORIZONTAL
            };

            GaussianBlurFilter(const GaussianBlurFilterBuilder*, BlurDirection);

            void onCameraProjectionUpdate(float, float);

        private:
            struct GaussianBlurShaderConst {
                uint32_t blurRadius;
                float maxBlurDistance;
            };

            std::string getShaderName() const override;
            void completeRenderer(const std::shared_ptr<GenericRendererBuilder>&, const std::shared_ptr<TextureReader>&) override;
            std::unique_ptr<ShaderConstants> buildShaderConstants() const override;

            static constexpr uint32_t CAMERA_PLANES_UNIFORM_BINDING = 0;
            static constexpr uint32_t BLUR_DATA_UNIFORM_BINDING = 1;
            static constexpr uint32_t SRC_TEX_UNIFORM_BINDING = 2;
            static constexpr uint32_t DEPTH_TEX_UNIFORM_BINDING = 3;

            std::shared_ptr<Texture> depthTexture; //source texture

            BlurDirection blurDirection;
            unsigned int blurRadius;
            float maxBlurDistance;

            unsigned int textureSize;
            struct {
                alignas(4) float nearPlane;
                alignas(4) float farPlane;
            } cameraPlanes;
    };

}
