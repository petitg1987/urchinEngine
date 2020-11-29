#include "GaussianBlurFilterBuilder.h"

namespace urchin {

    GaussianBlurFilterBuilder::GaussianBlurFilterBuilder() :
        TextureFilterBuilder(),
        pBlurDirection(BlurDirection::HORIZONTAL_BLUR),
        pBlurSize(3) //3x3 blur
    {

    }

    GaussianBlurFilterBuilder* GaussianBlurFilterBuilder::blurDirection(BlurDirection blurDirection) {
        this->pBlurDirection = blurDirection;
        return this;
    }

    GaussianBlurFilterBuilder* GaussianBlurFilterBuilder::blurSize(unsigned int blurSize) {
        this->pBlurSize = blurSize;
        return this;
    }

    unsigned int GaussianBlurFilterBuilder::getBlurSize() const {
        return pBlurSize;
    }

    std::unique_ptr<TextureFilter> GaussianBlurFilterBuilder::build() {
        std::unique_ptr<TextureFilter> textureFilter;
        if (pBlurDirection==BlurDirection::HORIZONTAL_BLUR) {
            textureFilter = std::make_unique<GaussianBlurFilter>(this, GaussianBlurFilter::HORIZONTAL);
        } else if (pBlurDirection==BlurDirection::VERTICAL_BLUR) {
            textureFilter = std::make_unique<GaussianBlurFilter>(this, GaussianBlurFilter::VERTICAL);
        } else {
            throw std::invalid_argument("Unknown blur direction type: " + std::to_string(pBlurDirection));
        }

        textureFilter->initialize();

        return textureFilter;
    }

    std::unique_ptr<GaussianBlurFilter> GaussianBlurFilterBuilder::buildGaussianBlur() {
        std::unique_ptr<TextureFilter> gaussianBlur = build();
        return std::unique_ptr<GaussianBlurFilter>(dynamic_cast<GaussianBlurFilter*>(gaussianBlur.release()));
    }

}
