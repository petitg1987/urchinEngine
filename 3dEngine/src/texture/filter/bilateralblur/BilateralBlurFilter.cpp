#include <stdexcept>

#include "BilateralBlurFilter.h"
#include "texture/filter/bilateralblur/BilateralBlurFilterBuilder.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    BilateralBlurFilter::BilateralBlurFilter(const BilateralBlurFilterBuilder* textureFilterBuilder, BlurDirection blurDirection):
            TextureFilter(textureFilterBuilder),
            depthTexture(textureFilterBuilder->getDepthTexture()),
            blurDirection(blurDirection),
            blurSize(textureFilterBuilder->getBlurSize()),
            blurSharpness(textureFilterBuilder->getBlurSharpness()),
            textureSize((BlurDirection::VERTICAL == blurDirection) ? getTextureHeight() : getTextureWidth()),
            cameraPlanes({}) {
        if (blurSize <= 1) {
            throw std::invalid_argument("Blur size must be greater than one. Value: " + std::to_string(blurSize));
        } else if (blurSize % 2 == 0) {
            throw std::invalid_argument("Blur size must be an odd number. Value: " + std::to_string(blurSize));
        }

        std::vector<float> offsets = computeOffsets();
        offsetsTab = toShaderVectorValues(offsets);
    }

    void BilateralBlurFilter::onCameraProjectionUpdate(float nearPlane, float farPlane) {
        cameraPlanes.nearPlane = nearPlane;
        cameraPlanes.farPlane = farPlane;

        getTextureRenderer()->updateShaderData(1, ShaderDataSender()
                .sendData(cameraNearPlaneShaderVar, cameraPlanes.nearPlane)
                .sendData(cameraFarPlaneShaderVar, cameraPlanes.farPlane));
    }

    std::string BilateralBlurFilter::getShaderName() const {
        return "bilateralBlurTex";
    }

    void BilateralBlurFilter::initiateAdditionalDisplay(const std::unique_ptr<GenericRendererBuilder>& textureRendererBuilder) {
        textureRendererBuilder
                ->addShaderData(ShaderDataSender()
                        .sendData(cameraNearPlaneShaderVar, cameraPlanes.nearPlane)
                        .sendData(cameraFarPlaneShaderVar, cameraPlanes.farPlane)) //binding 1
                ->addTextureReader(TextureReader::build(depthTexture, TextureParam::buildNearest()));
    }

    void BilateralBlurFilter::initiateAdditionalShaderVariables(const std::shared_ptr<Shader>& textureFilterShader) {
        cameraNearPlaneShaderVar = ShaderVar(textureFilterShader, "cameraNearPlane");
        cameraFarPlaneShaderVar = ShaderVar(textureFilterShader, "cameraFarPlane");

        int depthTexUnit = 1;
        ShaderDataSender().sendData(ShaderVar(textureFilterShader, "depthTex"), depthTexUnit); // binding 21
    }

    void BilateralBlurFilter::completeShaderTokens(std::map<std::string, std::string>& shaderTokens) const {
        shaderTokens["IS_VERTICAL_BLUR"] = (blurDirection == BlurDirection::VERTICAL) ? "true" : "false";
        shaderTokens["KERNEL_RADIUS"] = std::to_string(blurSize / 2);
        shaderTokens["BLUR_SHARPNESS"] = std::to_string(blurSharpness);
        shaderTokens["OFFSETS_TAB"] = offsetsTab;
    }

    std::vector<float> BilateralBlurFilter::computeOffsets() const {
        unsigned int numOffsets = blurSize / 2;
        std::vector<float> offsets(numOffsets, 0.0f);

        if (textureSize != 0) {
            float pixelSize = 1.0f / (float)textureSize;
            for (unsigned int i = 1; i <= numOffsets; ++i) {
                offsets[i - 1] = pixelSize * (float)i;
            }
        }

        return offsets;
    }
}
