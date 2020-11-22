#include "BilateralBlurFilterBuilder.h"

namespace urchin {

    BilateralBlurFilterBuilder::BilateralBlurFilterBuilder() :
        TextureFilterBuilder(),
        pBlurDirection(BlurDirection::HORIZONTAL_BLUR),
        pBlurSize(3), //3x3 blur
        pBlurSharpness(40.0),
        pDepthTextureID(0) {

    }

    BilateralBlurFilterBuilder *BilateralBlurFilterBuilder::blurDirection(BlurDirection blurDirection) {
        this->pBlurDirection = blurDirection;
        return this;
    }

    BilateralBlurFilterBuilder *BilateralBlurFilterBuilder::blurSize(unsigned int blurSize) {
        this->pBlurSize = blurSize;
        return this;
    }

    unsigned int BilateralBlurFilterBuilder::getBlurSize() const {
        return pBlurSize;
    }

    BilateralBlurFilterBuilder *BilateralBlurFilterBuilder::blurSharpness(float pBlurSharpness) {
        this->pBlurSharpness = pBlurSharpness;
        return this;
    }

    float BilateralBlurFilterBuilder::getBlurSharpness() const {
        return pBlurSharpness;
    }

    BilateralBlurFilterBuilder *BilateralBlurFilterBuilder::depthTextureID(unsigned int depthTextureID) {
        this->pDepthTextureID = depthTextureID;
        return this;
    }

    unsigned int BilateralBlurFilterBuilder::getDepthTextureID() const {
        return pDepthTextureID;
    }

    std::unique_ptr<TextureFilter> BilateralBlurFilterBuilder::build() {
        if (getTextureType()!=TextureType::DEFAULT) {
            throw std::invalid_argument("Unsupported texture type for bilateral blur filter: " + std::to_string(getTextureType()));
        } else if (getTextureFormat()!=GL_RED) {
            throw std::invalid_argument("Unsupported texture format for bilateral blur filter: " + std::to_string(getTextureType()));
        }

        std::unique_ptr<TextureFilter> textureFilter;
        if (pBlurDirection==BlurDirection::HORIZONTAL_BLUR) {
            textureFilter = std::make_unique<BilateralBlurFilter>(this, BilateralBlurFilter::HORIZONTAL);
        } else if (pBlurDirection==BlurDirection::VERTICAL_BLUR) {
            textureFilter = std::make_unique<BilateralBlurFilter>(this, BilateralBlurFilter::VERTICAL);
        } else {
            throw std::invalid_argument("Unknown blur direction type: " + std::to_string(pBlurDirection));
        }

        textureFilter->initialize();

        return textureFilter;
    }

    std::unique_ptr<BilateralBlurFilter> BilateralBlurFilterBuilder::buildBilateralBlur() {
        std::unique_ptr<TextureFilter> bilateralBlurFilter = build();
        return std::unique_ptr<BilateralBlurFilter>(dynamic_cast<BilateralBlurFilter*>(bilateralBlurFilter.release()));
    }

}
