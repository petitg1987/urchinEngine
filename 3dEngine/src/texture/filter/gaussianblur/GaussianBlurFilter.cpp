#include <stdexcept>

#include "texture/filter/gaussianblur/GaussianBlurFilter.h"
#include "texture/filter/gaussianblur/GaussianBlurFilterBuilder.h"
#include "graphics/render/GenericRendererBuilder.h"

namespace urchin {

    GaussianBlurFilter::GaussianBlurFilter(const GaussianBlurFilterBuilder* textureFilterBuilder, BlurDirection blurDirection):
            TextureFilter(textureFilterBuilder),
            depthTexture(textureFilterBuilder->getDepthTexture()),
            blurDirection(blurDirection),
            blurRadius(textureFilterBuilder->getBlurRadius()),
            maxBlurDistance(textureFilterBuilder->getMaxBlurDistance()),
            cameraNearPlane(0.01f),
            cameraFarPlane(100.0f),
            textureSize((VERTICAL == blurDirection) ? getTextureHeight() : getTextureWidth()) {
        if (blurRadius <= 0) {
            throw std::invalid_argument("Blur size must be greater than zero. Value: " + std::to_string(blurRadius));
        }
    }

    void GaussianBlurFilter::onCameraProjectionUpdate(float nearPlane, float farPlane) {
        if (cameraNearPlane != nearPlane || cameraFarPlane != farPlane) {
            cameraNearPlane = nearPlane;
            cameraFarPlane = farPlane;

            createOrUpdateRenderer();
        }
    }

    std::string GaussianBlurFilter::getShaderName() const {
        if (getTextureType() != TextureType::DEFAULT) {
            throw std::runtime_error("Unimplemented gaussian blur filter for texture type: " + std::to_string((int)getTextureType()));
        }

        if (getTextureFormat() == TextureFormat::GRAYSCALE_8_INT || getTextureFormat() == TextureFormat::GRAYSCALE_16_FLOAT || getTextureFormat() == TextureFormat::GRAYSCALE_32_FLOAT) {
            return "texFilterGaussianBlur";
        } else if (getTextureFormat() == TextureFormat::RGBA_8_INT || getTextureFormat() == TextureFormat::RGBA_16_FLOAT || getTextureFormat() == TextureFormat::RGBA_32_FLOAT) {
            return "texFilterGaussianBlurRgb";
        } else {
            throw std::runtime_error("Unimplemented gaussian blur filter for texture format: " + std::to_string((int)getTextureFormat()));
        }
    }

    void GaussianBlurFilter::completeRenderer(const std::shared_ptr<GenericRendererBuilder>& textureRendererBuilder, const std::shared_ptr<TextureReader>& sourceTextureReader) {
        textureRendererBuilder
                ->addUniformTextureReader(SRC_TEX_UNIFORM_BINDING, sourceTextureReader)
                ->addUniformTextureReader(DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildLinear()));
    }

    std::unique_ptr<ShaderConstants> GaussianBlurFilter::buildShaderConstants() const {
        GaussianBlurShaderConst gaussianBlurData {
            .isVerticalBlur = (blurDirection == VERTICAL) ? 1u : 0u,
            .blurRadius = blurRadius,
            .maxBlurDistance = maxBlurDistance,
            .cameraNearPlane = cameraNearPlane,
            .cameraFarPlane = cameraFarPlane
        };
        std::vector variablesSize = {
            sizeof(GaussianBlurShaderConst::isVerticalBlur),
            sizeof(GaussianBlurShaderConst::blurRadius),
            sizeof(GaussianBlurShaderConst::maxBlurDistance),
            sizeof(GaussianBlurShaderConst::cameraNearPlane),
            sizeof(GaussianBlurShaderConst::cameraFarPlane)
        };
        return std::make_unique<ShaderConstants>(variablesSize, &gaussianBlurData);
    }

}
