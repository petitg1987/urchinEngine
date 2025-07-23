#pragma once

#include <string>

#include "texture/filter/TextureFilter.h"

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
                uint32_t isVerticalBlur;
                uint32_t blurRadius;
                float maxBlurDistance;
                float cameraNearPlane;
                float cameraFarPlane;
            };

            std::string getShaderName() const override;
            void completeRenderer(const std::shared_ptr<GenericRendererBuilder>&, const std::shared_ptr<TextureReader>&) override;
            std::unique_ptr<ShaderConstants> buildShaderConstants() const override;

            static constexpr uint32_t SRC_TEX_UNIFORM_BINDING = 0;
            static constexpr uint32_t DEPTH_TEX_UNIFORM_BINDING = 1;

            std::shared_ptr<Texture> depthTexture; //source texture

            BlurDirection blurDirection;
            unsigned int blurRadius;
            float maxBlurDistance;
            float cameraNearPlane;
            float cameraFarPlane;

            unsigned int textureSize;
    };

}
