#include <stdexcept>

#include <texture/filter/gaussianblur/GaussianBlurFilter.h>
#include <texture/filter/gaussianblur/GaussianBlurFilterBuilder.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    GaussianBlurFilter::GaussianBlurFilter(const GaussianBlurFilterBuilder* textureFilterBuilder, BlurDirection blurDirection):
            TextureFilter(textureFilterBuilder),
            depthTexture(textureFilterBuilder->getDepthTexture()),
            blurDirection(blurDirection),
            blurRadius(textureFilterBuilder->getBlurRadius()),
            maxBlurDistance(textureFilterBuilder->getMaxBlurDistance()),
            textureSize((BlurDirection::VERTICAL == blurDirection) ? getTextureHeight() : getTextureWidth()),
            cameraPlanes({}) {
        if (blurRadius <= 0) {
            throw std::invalid_argument("Blur size must be greater than zero. Value: " + std::to_string(blurRadius));
        }
    }

    void GaussianBlurFilter::onCameraProjectionUpdate(float nearPlane, float farPlane) {
        cameraPlanes.nearPlane = nearPlane;
        cameraPlanes.farPlane = farPlane;

        getTextureRenderer().updateUniformData(CAMERA_PLANES_UNIFORM_BINDING, &cameraPlanes);
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
        Vector2<float> directionVector;
        if (blurDirection == VERTICAL) {
            directionVector = Vector2(0.0f, 1.0f);
        } else {
            directionVector = Vector2(1.0f, 0.0f);
        }

        textureRendererBuilder
                ->addUniformData(CAMERA_PLANES_UNIFORM_BINDING, sizeof(cameraPlanes), &cameraPlanes)
                ->addUniformData(BLUR_DATA_UNIFORM_BINDING, sizeof(directionVector), &directionVector)
                ->addUniformTextureReader(SRC_TEX_UNIFORM_BINDING, sourceTextureReader)
                ->addUniformTextureReader(DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildLinear()));
    }

    std::unique_ptr<ShaderConstants> GaussianBlurFilter::buildShaderConstants() const {
        GaussianBlurShaderConst gaussianBlurData{};
        gaussianBlurData.blurRadius = blurRadius;
        gaussianBlurData.maxBlurDistance = maxBlurDistance;
        std::vector variablesSize = {
                sizeof(GaussianBlurShaderConst::blurRadius),
                sizeof(GaussianBlurShaderConst::maxBlurDistance)
        };
        return std::make_unique<ShaderConstants>(variablesSize, &gaussianBlurData);
    }

}
