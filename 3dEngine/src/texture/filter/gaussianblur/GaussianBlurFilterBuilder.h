#pragma once

#include "texture/filter/TextureFilterBuilder.h"
#include "texture/filter/gaussianblur/GaussianBlurFilter.h"

namespace urchin {

    class GaussianBlurFilterBuilder final : public TextureFilterBuilder<GaussianBlurFilterBuilder> {
        public:
            enum BlurDirection {
                HORIZONTAL_BLUR,
                VERTICAL_BLUR
            };

            GaussianBlurFilterBuilder(bool, std::string, const std::shared_ptr<Texture>&);
            ~GaussianBlurFilterBuilder() override = default;

            GaussianBlurFilterBuilder* blurDirection(BlurDirection);

            GaussianBlurFilterBuilder* blurRadius(unsigned int);
            unsigned int getBlurRadius() const;

            GaussianBlurFilterBuilder* maxBlurDistance(float);
            float getMaxBlurDistance() const;

            GaussianBlurFilterBuilder* depthTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getDepthTexture() const;

            std::unique_ptr<TextureFilter> build() override;
            std::unique_ptr<GaussianBlurFilter> buildGaussianBlur();

        private:
            BlurDirection pBlurDirection;
            unsigned int pBlurRadius;
            float pMaxBlurDistance;
            std::shared_ptr<Texture> pDepthTexture;
    };

}
