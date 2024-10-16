#pragma once

#include <texture/filter/TextureFilterBuilder.h>

namespace urchin {

    class GaussianBlur3dFilterBuilder final : public TextureFilterBuilder<GaussianBlur3dFilterBuilder> {
        public:
            enum BlurDirection {
                HORIZONTAL_BLUR,
                VERTICAL_BLUR
            };

            GaussianBlur3dFilterBuilder(bool, std::string, const std::shared_ptr<Texture>&);
            ~GaussianBlur3dFilterBuilder() override = default;

            GaussianBlur3dFilterBuilder* blurDirection(BlurDirection);

            GaussianBlur3dFilterBuilder* blurSize(unsigned int);
            unsigned int getBlurSize() const;

            GaussianBlur3dFilterBuilder* blurSharpness(float);
            float getBlurSharpness() const;

            GaussianBlur3dFilterBuilder* depthTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getDepthTexture() const;

            std::unique_ptr<TextureFilter> build() override;
            std::unique_ptr<GaussianBlur3dFilter> buildGaussianBlur3d();

        private:
            BlurDirection pBlurDirection;
            unsigned int pBlurSize;
            float pBlurSharpness;
            std::shared_ptr<Texture> pDepthTexture;
    };

}
