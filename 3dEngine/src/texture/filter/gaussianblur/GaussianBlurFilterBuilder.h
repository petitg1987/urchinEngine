#ifndef URCHINENGINE_GAUSSIANBLURFILTERBUILDER_H
#define URCHINENGINE_GAUSSIANBLURFILTERBUILDER_H

#include "texture/filter/TextureFilterBuilder.h"

namespace urchin {

    class GaussianBlurFilterBuilder : public TextureFilterBuilder<GaussianBlurFilterBuilder> {
        public:
            enum BlurDirection {
                HORIZONTAL_BLUR,
                VERTICAL_BLUR
            };

            GaussianBlurFilterBuilder();
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

#endif
