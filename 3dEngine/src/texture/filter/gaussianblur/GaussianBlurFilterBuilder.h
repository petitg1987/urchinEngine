#pragma once

#include <texture/filter/TextureFilterBuilder.h>

namespace urchin {

    class GaussianBlurFilterBuilder : public TextureFilterBuilder<GaussianBlurFilterBuilder> {
        public:
            enum BlurDirection {
                HORIZONTAL_BLUR,
                VERTICAL_BLUR
            };

            explicit GaussianBlurFilterBuilder(bool, std::string, const std::shared_ptr<Texture>&);
            ~GaussianBlurFilterBuilder() override = default;

            GaussianBlurFilterBuilder* blurDirection(BlurDirection);

            GaussianBlurFilterBuilder* blurSize(unsigned int);
            unsigned int getBlurSize() const;

            std::unique_ptr<TextureFilter> build() override;
            std::unique_ptr<GaussianBlurFilter> buildGaussianBlur();

        private:
            BlurDirection pBlurDirection;
            unsigned int pBlurSize;
    };

}
