#include <stdexcept>
#include <cmath>
#include <algorithm>

#include "scene/ui/NineSlicingTexture.h"
#include "resources/ResourceRetriever.h"

namespace urchin {
//TODO GPE review:
    std::shared_ptr<Texture> NineSlicingTexture::build(const std::shared_ptr<Image>& rawImage, unsigned int width, unsigned int height, const Outline& outline) const {
        return build(rawImage, width, height, outline, outline);
    }

    std::shared_ptr<Texture> NineSlicingTexture::build(const std::shared_ptr<Image>& rawImage, unsigned int width, unsigned int height, const Outline& sourceOutline, const Outline& targetOutline) const {
        if (width < targetOutline.leftPixel + targetOutline.rightPixel) {
            throw std::runtime_error("Nine slicing texture width (" + std::to_string(width) + ") is smaller than left + right outline width (" + std::to_string(targetOutline.leftPixel + targetOutline.rightPixel) + ") for image: " + rawImage->getName());
        } else if (height < targetOutline.topPixel + targetOutline.bottomPixel) {
            throw std::runtime_error("Nine slicing texture height (" + std::to_string(height) + ") is smaller than top + bottom outline width (" + std::to_string(targetOutline.topPixel + targetOutline.bottomPixel) + ") for image: " + rawImage->getName());
        } else if ((sourceOutline.leftPixel == 0 && targetOutline.leftPixel != 0) ||
                (sourceOutline.rightPixel == 0 && targetOutline.rightPixel != 0) ||
                (sourceOutline.topPixel == 0 && targetOutline.topPixel != 0) ||
                (sourceOutline.bottomPixel == 0 && targetOutline.bottomPixel != 0)) {
            throw std::runtime_error("Nine slicing target outline cannot be defined where the source outline is zero for image: " + rawImage->getName());
        }

        unsigned int componentsCount = rawImage->retrieveComponentsCount();
        const std::vector<unsigned char>& rawTexels = rawImage->getTexels();
        unsigned int rawWidth = rawImage->getWidth();
        unsigned int rawHeight = rawImage->getHeight();
        if (rawWidth <= sourceOutline.leftPixel + sourceOutline.rightPixel) {
            throw std::runtime_error("Nine slicing source image width (" + std::to_string(rawWidth) + ") must be greater than left + right outline width (" + std::to_string(sourceOutline.leftPixel + sourceOutline.rightPixel) + ") for image: " + rawImage->getName());
        } else if (rawHeight <= sourceOutline.topPixel + sourceOutline.bottomPixel) {
            throw std::runtime_error("Nine slicing source image height (" + std::to_string(rawHeight) + ") must be greater than top + bottom outline width (" + std::to_string(sourceOutline.topPixel + sourceOutline.bottomPixel) + ") for image: " + rawImage->getName());
        }

        //for each target column/row: source columns/rows with their weight (outlines are scaled, middle parts are repeated)
        std::vector<std::vector<SourceTexelWeight>> columnsMapping = computeAxisMapping(width, rawWidth, sourceOutline.leftPixel, sourceOutline.rightPixel, targetOutline.leftPixel, targetOutline.rightPixel);
        std::vector<std::vector<SourceTexelWeight>> rowsMapping = computeAxisMapping(height, rawHeight, sourceOutline.topPixel, sourceOutline.bottomPixel, targetOutline.topPixel, targetOutline.bottomPixel);

        std::vector<unsigned char> texels(height * width * componentsCount, 0);
        std::vector<float> texelComponents(componentsCount);
        for (unsigned int y = 0; y < height; ++y) {
            for (unsigned int x = 0; x < width; ++x) {
                std::ranges::fill(texelComponents, 0.0f);
                for (const SourceTexelWeight& row : rowsMapping[y]) {
                    for (const SourceTexelWeight& column : columnsMapping[x]) {
                        float weight = row.weight * column.weight;
                        std::size_t rawTexelIndex = ((std::size_t)row.index * rawWidth + column.index) * componentsCount;
                        for (unsigned int c = 0; c < componentsCount; ++c) {
                            texelComponents[c] += weight * (float)rawTexels[rawTexelIndex + c];
                        }
                    }
                }

                std::size_t texelIndex = ((std::size_t)y * width + x) * componentsCount;
                for (unsigned int c = 0; c < componentsCount; ++c) {
                    texels[texelIndex + c] = (unsigned char)std::clamp(std::lround(texelComponents[c]), 0l, 255l);
                }
            }
        }

        //create the texture
        return Texture::build("9-slicing widget", width, height, rawImage->retrieveTextureFormat(), texels.data(), rawImage->getTransparencyData(), TextureDataType::INT_8);
    }

    std::vector<std::vector<NineSlicingTexture::SourceTexelWeight>> NineSlicingTexture::computeAxisMapping(unsigned int targetSize, unsigned int sourceSize,
            unsigned int sourceStartOutline, unsigned int sourceEndOutline, unsigned int targetStartOutline, unsigned int targetEndOutline) {
        std::vector<std::vector<SourceTexelWeight>> axisMapping(targetSize);
        unsigned int targetEndOutlineStart = targetSize - targetEndOutline;
        unsigned int sourceEndOutlineStart = sourceSize - sourceEndOutline;
        unsigned int sourceMiddleSize = sourceSize - (sourceStartOutline + sourceEndOutline);

        for (unsigned int t = 0; t < targetSize; ++t) {
            if (t < targetStartOutline) {
                addScaledMapping(axisMapping[t], t, 0, sourceStartOutline, targetStartOutline);
            } else if (t >= targetEndOutlineStart) {
                addScaledMapping(axisMapping[t], t - targetEndOutlineStart, sourceEndOutlineStart, sourceEndOutline, targetEndOutline);
            } else {
                axisMapping[t].push_back({.index = sourceStartOutline + (t - targetStartOutline) % sourceMiddleSize, .weight = 1.0f});
            }
        }
        return axisMapping;
    }

    /**
     * Box filter: the target texel is the weighted average of the source texels it covers
     */
    void NineSlicingTexture::addScaledMapping(std::vector<SourceTexelWeight>& texelMapping, unsigned int targetIndex, unsigned int sourceStart, unsigned int sourceSize, unsigned int targetSize) {
        float ratio = (float)sourceSize / (float)targetSize;
        float begin = (float)targetIndex * ratio;
        float end = std::min((float)(targetIndex + 1) * ratio, (float)sourceSize);

        auto firstSource = (unsigned int)begin;
        auto lastSource = std::min((unsigned int)std::ceil(end), sourceSize);
        for (unsigned int s = firstSource; s < lastSource; ++s) {
            float overlap = std::min(end, (float)(s + 1)) - std::max(begin, (float)s);
            if (overlap > 0.0f) {
                texelMapping.push_back({.index = sourceStart + s, .weight = overlap / ratio});
            }
        }
    }

}
