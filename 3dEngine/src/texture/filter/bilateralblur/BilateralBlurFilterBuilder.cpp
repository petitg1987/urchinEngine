#include <texture/filter/bilateralblur/BilateralBlurFilterBuilder.h>

namespace urchin {

    BilateralBlurFilterBuilder::BilateralBlurFilterBuilder(bool bUseNullRenderTarget, std::string name, const std::shared_ptr<Texture>& sourceTexture) :
            TextureFilterBuilder(bUseNullRenderTarget, std::move(name), sourceTexture),
            pBlurDirection(BlurDirection::HORIZONTAL_BLUR),
            pBlurSize(3), //3x3 blur
            pBlurSharpness(40.0) {

    }

    BilateralBlurFilterBuilder* BilateralBlurFilterBuilder::blurDirection(BlurDirection blurDirection) {
        this->pBlurDirection = blurDirection;
        return this;
    }

    BilateralBlurFilterBuilder* BilateralBlurFilterBuilder::blurSize(unsigned int blurSize) {
        this->pBlurSize = blurSize;
        return this;
    }

    unsigned int BilateralBlurFilterBuilder::getBlurSize() const {
        return pBlurSize;
    }

    BilateralBlurFilterBuilder* BilateralBlurFilterBuilder::blurSharpness(float pBlurSharpness) {
        this->pBlurSharpness = pBlurSharpness;
        return this;
    }

    float BilateralBlurFilterBuilder::getBlurSharpness() const {
        return pBlurSharpness;
    }

    BilateralBlurFilterBuilder* BilateralBlurFilterBuilder::depthTexture(const std::shared_ptr<Texture>& depthTexture) {
        this->pDepthTexture = depthTexture;
        return this;
    }

    const std::shared_ptr<Texture>& BilateralBlurFilterBuilder::getDepthTexture() const {
        return pDepthTexture;
    }

    std::unique_ptr<TextureFilter> BilateralBlurFilterBuilder::build() {
        assert(getDepthTexture());

        if (getTextureType() != TextureType::DEFAULT) {
            throw std::invalid_argument("Unsupported texture type for bilateral blur filter: " + std::to_string((int)getTextureType()));
        } else if (getTextureFormat() != TextureFormat::GRAYSCALE_8_INT && getTextureFormat() != TextureFormat::GRAYSCALE_16_FLOAT && getTextureFormat() != TextureFormat::GRAYSCALE_32_FLOAT
                && getTextureFormat() != TextureFormat::RGBA_8_INT && getTextureFormat() != TextureFormat::RGBA_16_FLOAT && getTextureFormat() != TextureFormat::RGBA_32_FLOAT) {
            throw std::invalid_argument("Unsupported texture format for bilateral blur filter: " + std::to_string((int)getTextureFormat()));
        }

        std::unique_ptr<TextureFilter> textureFilter;
        if (pBlurDirection == BlurDirection::HORIZONTAL_BLUR) {
            textureFilter = std::make_unique<GaussianBlur3dFilter>(this, GaussianBlur3dFilter::HORIZONTAL);
        } else if (pBlurDirection == BlurDirection::VERTICAL_BLUR) {
            textureFilter = std::make_unique<GaussianBlur3dFilter>(this, GaussianBlur3dFilter::VERTICAL);
        } else {
            throw std::invalid_argument("Unknown blur direction type: " + std::to_string(pBlurDirection));
        }

        textureFilter->initialize();

        return textureFilter;
    }

    std::unique_ptr<GaussianBlur3dFilter> BilateralBlurFilterBuilder::buildBilateralBlur() {
        std::unique_ptr<TextureFilter> bilateralBlurFilter = build();
        return std::unique_ptr<GaussianBlur3dFilter>(static_cast<GaussianBlur3dFilter*>(bilateralBlurFilter.release()));
    }

}
