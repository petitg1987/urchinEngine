#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <UrchinCommon.h>

#include <texture/filter/gaussianblur/GaussianBlurFilter.h>
#include <texture/filter/gaussianblur/GaussianBlurFilterBuilder.h>
#include <api/render/GenericRendererBuilder.h>

namespace urchin {

    GaussianBlurFilter::GaussianBlurFilter(const GaussianBlurFilterBuilder* textureFilterBuilder, BlurDirection blurDirection):
        TextureFilter(textureFilterBuilder),
        blurDirection(blurDirection),
        blurSize(textureFilterBuilder->getBlurSize()),
        nbTextureFetch(MathFunction::ceilToUInt((float)blurSize / 2.0f)),
        textureSize((BlurDirection::VERTICAL == blurDirection) ? getTextureHeight() : getTextureWidth()) { //See http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

        if (blurSize <= 1) {
            throw std::invalid_argument("Blur size must be greater than one. Value: " + std::to_string(blurSize));
        } else if (blurSize % 2 == 0) {
            throw std::invalid_argument("Blur size must be an odd number. Value: " + std::to_string(blurSize));
        }

        if (nbTextureFetch > TEXTURE_FETCH_SHADER_LIMIT) {
            throw std::invalid_argument("Texture fetch value is limited to " + std::to_string(TEXTURE_FETCH_SHADER_LIMIT) + ". Value: " + std::to_string(nbTextureFetch));
        }

        std::vector<float> weights = computeWeights();
        weightsLinearSampling = computeWeightsLinearSampling(weights);
        offsetsLinearSampling = computeOffsetsLinearSampling(weights, weightsLinearSampling);
    }

    std::string GaussianBlurFilter::getShaderName() const {
        if (getTextureFormat() == TextureFormat::RG_32_FLOAT && getTextureType() == TextureType::ARRAY) {
            return "texFilterGaussianBlurVec2Array";
        }
        throw std::runtime_error("Unimplemented gaussian blur filter for: " + std::to_string((int)getTextureFormat()) + " - " + std::to_string((int)getTextureType()));
    }

    void GaussianBlurFilter::completeRenderer(const std::shared_ptr<GenericRendererBuilder>& textureRendererBuilder, const std::shared_ptr<TextureReader>& sourceTextureReader) {
        std::vector<float> offsetsShaderData(offsetsLinearSampling.size() * 4, 0.0f);
        for (std::size_t i = 0; i < offsetsLinearSampling.size(); ++i) {
            offsetsShaderData[i * 4] = offsetsLinearSampling[i];
        }

        std::vector<float> weightsShaderData(weightsLinearSampling.size() * 4, 0.0f);
        for (std::size_t i = 0; i < weightsLinearSampling.size(); ++i) {
            weightsShaderData[i * 4] = weightsLinearSampling[i];
        }

        textureRendererBuilder
                ->addUniformData(offsetsShaderData.size() * sizeof(float), offsetsShaderData.data()) //binding 1
                ->addUniformData(weightsShaderData.size() * sizeof(float), weightsShaderData.data()) //binding 2
                ->addUniformTextureReader(sourceTextureReader); //binding 3
    }

    std::unique_ptr<ShaderConstants> GaussianBlurFilter::buildShaderConstants() const {
        GaussianBlurShaderConst gaussianBlurData{};
        gaussianBlurData.numberLayer = getTextureLayer();
        gaussianBlurData.isVerticalBlur = blurDirection == BlurDirection::VERTICAL;
        gaussianBlurData.nbTextureFetch = nbTextureFetch;
        std::vector<std::size_t> variablesSize = {
                sizeof(GaussianBlurShaderConst::numberLayer),
                sizeof(GaussianBlurShaderConst::isVerticalBlur),
                sizeof(GaussianBlurShaderConst::nbTextureFetch)
        };
        return std::make_unique<ShaderConstants>(variablesSize, &gaussianBlurData);
    }

    std::vector<float> GaussianBlurFilter::computeWeights() const {
        std::vector<unsigned int> pascalTriangleLineValues = PascalTriangle::lineValues(blurSize + 3);

        //exclude outermost because the values are too insignificant and haven't enough impact on 32 bits value
        std::vector<float> gaussianFactors(blurSize);
        std::copy_n(pascalTriangleLineValues.begin() + 2, blurSize, gaussianFactors.begin());

        float factorsSum = std::accumulate(gaussianFactors.begin(), gaussianFactors.end(), 0.0f);
        std::vector<float> weights(blurSize);
        for (unsigned int i = 0; i < blurSize; ++i) {
            weights[i] = gaussianFactors[i] / factorsSum;
        }

        return weights;
    }

    std::vector<float> GaussianBlurFilter::computeWeightsLinearSampling(const std::vector<float>& weights) const {
        std::vector<float> weightsLinearSampling(nbTextureFetch);

        for (unsigned int i = 0; i < nbTextureFetch; ++i) {
            if (i * 2 + 1 >= blurSize) {
                weightsLinearSampling[i] = weights[i * 2];
            } else {
                weightsLinearSampling[i] = weights[i * 2] + weights[i * 2 + 1];
            }
        }

        return weightsLinearSampling;
    }

    std::vector<float> GaussianBlurFilter::computeOffsetsLinearSampling(const std::vector<float>& weights,
            const std::vector<float>& weightsLinearSampling) const {
        std::vector<float> offsetsLinearSampling(nbTextureFetch);

        int firstOffset = -MathFunction::floorToInt((float)blurSize / 2.0f);
        for (unsigned int i = 0; i < nbTextureFetch; ++i) {
            if (i * 2 + 1 >= blurSize) {
                offsetsLinearSampling[i] = (float)(firstOffset + (int)i * 2);
            } else {
                int offset1 = firstOffset + (int)i * 2;
                int offset2 = offset1 + 1;
                offsetsLinearSampling[i] = ((float)offset1 * weights[i * 2] + (float)offset2 * weights[i * 2 + 1]) / weightsLinearSampling[i];
            }

            offsetsLinearSampling[i] /= (float)textureSize;
        }

        return offsetsLinearSampling;
    }

}
