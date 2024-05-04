#include <stdexcept>

#include <texture/filter/bilateralblur/BilateralBlurFilter.h>
#include <texture/filter/bilateralblur/BilateralBlurFilterBuilder.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    BilateralBlurFilter::BilateralBlurFilter(const BilateralBlurFilterBuilder* textureFilterBuilder, BlurDirection blurDirection):
            TextureFilter(textureFilterBuilder),
            depthTexture(textureFilterBuilder->getDepthTexture()),
            blurDirection(blurDirection),
            blurSharpness(textureFilterBuilder->getBlurSharpness()),
            textureSize((BlurDirection::VERTICAL == blurDirection) ? getTextureHeight() : getTextureWidth()),
            cameraPlanes({}) {
        unsigned int blurSize = textureFilterBuilder->getBlurSize();
        if (blurSize <= 1) {
            throw std::invalid_argument("Blur size must be greater than one. Value: " + std::to_string(blurSize));
        } else if (blurSize % 2 == 0) {
            throw std::invalid_argument("Blur size must be an odd number. Value: " + std::to_string(blurSize));
        }

        kernelRadius = textureFilterBuilder->getBlurSize() / 2;
        if (kernelRadius > KERNEL_RADIUS_SHADER_LIMIT) {
            throw std::invalid_argument("Kernel radius value is limited to " + std::to_string(KERNEL_RADIUS_SHADER_LIMIT) + ". Value: " + std::to_string(kernelRadius));
        }

        offsets = computeOffsets();
    }

    void BilateralBlurFilter::onCameraProjectionUpdate(float nearPlane, float farPlane) {
        cameraPlanes.nearPlane = nearPlane;
        cameraPlanes.farPlane = farPlane;

        getTextureRenderer().updateUniformData(CAMERA_PLANES_UNIFORM_BINDING, &cameraPlanes);
    }

    std::string BilateralBlurFilter::getShaderName() const {
        if ((getTextureFormat() == TextureFormat::GRAYSCALE_8_INT || getTextureFormat() == TextureFormat::GRAYSCALE_16_FLOAT || getTextureFormat() == TextureFormat::GRAYSCALE_32_FLOAT)
                && getTextureType() == TextureType::DEFAULT) {
            return "texFilterBilateralBlur";
        }
        throw std::runtime_error("Unimplemented bilateral blur filter for: " + std::to_string((int)getTextureFormat()) + " - " + std::to_string((int)getTextureType()));
    }

    void BilateralBlurFilter::completeRenderer(const std::shared_ptr<GenericRendererBuilder>& textureRendererBuilder, const std::shared_ptr<TextureReader>& sourceTextureReader) {
        std::vector<float> offsetsShaderData(offsets.size() * 4, 0.0f);
        for (std::size_t i = 0; i< offsets.size(); ++i) {
            offsetsShaderData[i * 4] = offsets[i];
        }

        textureRendererBuilder
                ->addUniformData(CAMERA_PLANES_UNIFORM_BINDING, sizeof(cameraPlanes), &cameraPlanes)
                ->addUniformData(OFFSETS_DATA_UNIFORM_BINDING, offsetsShaderData.size() * sizeof(float), offsetsShaderData.data())
                ->addUniformTextureReader(SRC_TEX_UNIFORM_BINDING, sourceTextureReader)
                ->addUniformTextureReader(DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildNearest()));
    }

    std::unique_ptr<ShaderConstants> BilateralBlurFilter::buildShaderConstants() const {
        BilateralBlurShaderConst bilateralBlurData{};
        bilateralBlurData.isVerticalBlur = blurDirection == BlurDirection::VERTICAL;
        bilateralBlurData.kernelRadius = kernelRadius;
        bilateralBlurData.blurSharpness = blurSharpness;
        std::vector<std::size_t> variablesSize = {
                sizeof(BilateralBlurShaderConst::isVerticalBlur),
                sizeof(BilateralBlurShaderConst::kernelRadius),
                sizeof(BilateralBlurShaderConst::blurSharpness)
        };
        return std::make_unique<ShaderConstants>(variablesSize, &bilateralBlurData);
    }

    std::vector<float> BilateralBlurFilter::computeOffsets() const {
        std::vector<float> offsets(kernelRadius, 0.0f);

        if (textureSize != 0) {
            float pixelSize = 1.0f / (float)textureSize;
            for (unsigned int i = 1; i <= kernelRadius; ++i) {
                offsets[i - 1] = pixelSize * (float)i;
            }
        }

        return offsets;
    }
}
