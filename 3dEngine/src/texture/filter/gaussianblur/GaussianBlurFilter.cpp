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

        computeUvOffsets();
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
        std::vector<float> uvOffsetsShaderData(uvOffsets.size() * 4, 0.0f);
        for (std::size_t i = 0; i < uvOffsets.size(); ++i) {
            uvOffsetsShaderData[i * 4] = uvOffsets[i].X;
            uvOffsetsShaderData[(i * 4) + 1] = uvOffsets[i].Y;
        }

        textureRendererBuilder
                ->addUniformData(CAMERA_PLANES_UNIFORM_BINDING, sizeof(cameraPlanes), &cameraPlanes)
                ->addUniformData(OFFSETS_DATA_UNIFORM_BINDING, uvOffsetsShaderData.size() * sizeof(float), uvOffsetsShaderData.data())
                ->addUniformTextureReader(SRC_TEX_UNIFORM_BINDING, sourceTextureReader)
                ->addUniformTextureReader(DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildNearest()));
    }

    std::unique_ptr<ShaderConstants> GaussianBlurFilter::buildShaderConstants() const {
        GaussianBlurShaderConst gaussianBlurData{};
        gaussianBlurData.kernelRadius = kernelRadius;
        gaussianBlurData.blurSharpness = blurSharpness;
        std::vector<std::size_t> variablesSize = {
                sizeof(GaussianBlurShaderConst::kernelRadius),
                sizeof(GaussianBlurShaderConst::blurSharpness)
        };
        return std::make_unique<ShaderConstants>(variablesSize, &gaussianBlurData);
    }

    void GaussianBlurFilter::computeUvOffsets() {
        uvOffsets.resize(kernelRadius, Vector2<float>(0.0f, 0.0f));

        if (textureSize != 0) {
            float pixelSize = 1.0f / (float)textureSize;
            for (unsigned int i = 1; i <= kernelRadius; ++i) {
                if (blurDirection == BlurDirection::VERTICAL) {
                    uvOffsets[i - 1] = Vector2<float>(0.0f, pixelSize * (float)i);
                } else {
                    uvOffsets[i - 1] = Vector2<float>(pixelSize * (float)i, 0.0f);
                }
            }
        }
    }
}
