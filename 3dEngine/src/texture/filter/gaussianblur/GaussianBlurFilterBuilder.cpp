#include <texture/filter/gaussianblur/GaussianBlurFilterBuilder.h>

namespace urchin {

    GaussianBlurFilterBuilder::GaussianBlurFilterBuilder(bool isTestMode, std::string name, const std::shared_ptr<Texture>& sourceTexture) :
            TextureFilterBuilder(isTestMode, std::move(name), sourceTexture),
            pBlurDirection(BlurDirection::HORIZONTAL_BLUR),
            pBlurRadius(3),
            pMaxBlurDistance(10.0) {

    }

    GaussianBlurFilterBuilder* GaussianBlurFilterBuilder::blurDirection(BlurDirection blurDirection) {
        this->pBlurDirection = blurDirection;
        return this;
    }

    GaussianBlurFilterBuilder* GaussianBlurFilterBuilder::blurRadius(unsigned int blurRadius) {
        this->pBlurRadius = blurRadius;
        return this;
    }

    unsigned int GaussianBlurFilterBuilder::getBlurRadius() const {
        return pBlurRadius;
    }

    GaussianBlurFilterBuilder* GaussianBlurFilterBuilder::maxBlurDistance(float maxBlurDistance) {
        this->pMaxBlurDistance = maxBlurDistance;
        return this;
    }

    float GaussianBlurFilterBuilder::getMaxBlurDistance() const {
        return pMaxBlurDistance;
    }

    GaussianBlurFilterBuilder* GaussianBlurFilterBuilder::depthTexture(const std::shared_ptr<Texture>& depthTexture) {
        this->pDepthTexture = depthTexture;
        return this;
    }

    const std::shared_ptr<Texture>& GaussianBlurFilterBuilder::getDepthTexture() const {
        return pDepthTexture;
    }

    std::unique_ptr<TextureFilter> GaussianBlurFilterBuilder::build() {
        assert(getDepthTexture());

        if (getTextureType() != TextureType::DEFAULT) {
            throw std::invalid_argument("Unsupported texture type for gaussian blur 3d filter: " + std::to_string((int)getTextureType()));
        } else if (getTextureFormat() != TextureFormat::GRAYSCALE_8_INT && getTextureFormat() != TextureFormat::GRAYSCALE_16_FLOAT && getTextureFormat() != TextureFormat::GRAYSCALE_32_FLOAT
                && getTextureFormat() != TextureFormat::RGBA_8_INT && getTextureFormat() != TextureFormat::RGBA_16_FLOAT && getTextureFormat() != TextureFormat::RGBA_32_FLOAT) {
            throw std::invalid_argument("Unsupported texture format for gaussian blur 3d filter: " + std::to_string((int)getTextureFormat()));
        }

        std::unique_ptr<TextureFilter> textureFilter;
        if (pBlurDirection == BlurDirection::HORIZONTAL_BLUR) {
            textureFilter = std::make_unique<GaussianBlurFilter>(this, GaussianBlurFilter::HORIZONTAL);
        } else if (pBlurDirection == BlurDirection::VERTICAL_BLUR) {
            textureFilter = std::make_unique<GaussianBlurFilter>(this, GaussianBlurFilter::VERTICAL);
        } else {
            throw std::invalid_argument("Unknown blur direction type: " + std::to_string(pBlurDirection));
        }

        textureFilter->initialize();

        return textureFilter;
    }

    std::unique_ptr<GaussianBlurFilter> GaussianBlurFilterBuilder::buildGaussianBlur() {
        std::unique_ptr<TextureFilter> gaussianBlurFilter = build();
        return std::unique_ptr<GaussianBlurFilter>(static_cast<GaussianBlurFilter*>(gaussianBlurFilter.release()));
    }

}
