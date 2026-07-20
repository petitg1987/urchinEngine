#include "scene/ui/NineSlicingTexture.h"

#include "resources/ResourceRetriever.h"

namespace urchin {

    std::shared_ptr<Texture> NineSlicingTexture::build(const std::shared_ptr<Image>& rawImage, unsigned int width, unsigned int height, const Outline& outline) const {
        unsigned int componentsCount = rawImage->retrieveComponentsCount();
        const std::vector<unsigned char>& rawTexels = rawImage->getTexels();
        unsigned int rawWidth = rawImage->getWidth();
        unsigned int rawHeight = rawImage->getHeight();

        std::vector<unsigned char> texels(height * width * componentsCount, 0);

        auto widthMinusRight = std::max(0u, (unsigned int)((int)width - (int)outline.rightWidth));
        auto heightMinusBottom = std::max(0u, (unsigned int)((int)height - (int)outline.bottomWidth));
        unsigned int leftMultiplyInternalFormat = std::min(width, outline.leftWidth) * componentsCount;
        unsigned int topAdjusted = std::min(height, outline.topWidth);

        //copy corner top left
        for (unsigned int i = 0; i < topAdjusted; ++i) {
            for (unsigned int j = 0; j < leftMultiplyInternalFormat; ++j) {
                texels[i * width * componentsCount + j] = rawTexels[i * rawWidth * componentsCount + j];
            }
        }

        //copy top
        for (unsigned int i = 0; i < topAdjusted; ++i) {
            for (unsigned int j = outline.leftWidth * componentsCount, k = 0; j < widthMinusRight * componentsCount; ++j, ++k) {
                texels[i * width * componentsCount + j] = rawTexels[i * rawWidth * componentsCount + k % ((rawWidth - (outline.rightWidth + outline.leftWidth)) * componentsCount)
                    + outline.leftWidth * componentsCount];
            }
        }

        //copy corner top right
        for (unsigned int i = 0; i < topAdjusted; ++i) {
            for (unsigned int j = widthMinusRight * componentsCount, k = (rawWidth - outline.rightWidth) * componentsCount; j < width * componentsCount; ++j, ++k) {
                texels[i * width * componentsCount + j] = rawTexels[i * rawWidth * componentsCount + k];
            }
        }

        //copy right
        for (unsigned int i = outline.topWidth, k = 0; i < heightMinusBottom; ++i, ++k) {
            for (unsigned int j = widthMinusRight * componentsCount, l = (rawWidth - outline.rightWidth) * componentsCount; j < width * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawTexels[(k % (rawHeight - (outline.topWidth + outline.bottomWidth)) + outline.topWidth) * rawWidth * componentsCount + l];
            }
        }

        //copy corner bottom right
        for (unsigned int i = heightMinusBottom, k = rawHeight - outline.bottomWidth; i < height; ++i, ++k) {
            for (unsigned int j = widthMinusRight * componentsCount, l = (rawWidth - outline.rightWidth) * componentsCount; j < width * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawTexels[k * rawWidth * componentsCount + l];
            }
        }

        //copy bottom
        for (unsigned int i = heightMinusBottom, k = rawHeight - outline.bottomWidth; i < height; ++i, ++k) {
            for (unsigned int j = outline.leftWidth * componentsCount, l = 0; j < widthMinusRight * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawTexels[k * rawWidth * componentsCount + l % ((rawWidth - (outline.leftWidth + outline.rightWidth)) * componentsCount)
                    + outline.leftWidth * componentsCount];
            }
        }

        //copy corner bottom left
        for (unsigned int i = heightMinusBottom, k = 0; i < height; ++i, ++k) {
            for (unsigned int j = 0; j < leftMultiplyInternalFormat; ++j) {
                texels[i * width * componentsCount + j] = rawTexels[(k + rawHeight - outline.bottomWidth) * rawWidth * componentsCount + j];
            }
        }

        //copy left
        for (unsigned int i = outline.topWidth, k = 0;i < heightMinusBottom;++i, ++k) {
            for (unsigned int j = 0; j < leftMultiplyInternalFormat; ++j) {
                texels[i * width * componentsCount + j] = rawTexels[(k % (rawHeight - (outline.topWidth + outline.bottomWidth)) + outline.topWidth) * rawWidth * componentsCount + j];
            }
        }

        //copy middle
        for (unsigned int i = outline.topWidth, k = 0; i < heightMinusBottom; ++i, ++k) {
            for (unsigned int j = leftMultiplyInternalFormat, l = 0; j < widthMinusRight * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawTexels[(k % (rawHeight - (outline.topWidth + outline.bottomWidth)) + outline.topWidth) * rawWidth * componentsCount
                    + l % ((rawWidth - (outline.leftWidth + outline.rightWidth)) * componentsCount) + outline.leftWidth * componentsCount];
            }
        }

        //create the texture
        return Texture::build("9-slicing widget", width, height, rawImage->retrieveTextureFormat(), texels.data(), rawImage->hasTransparency(), TextureDataType::INT_8);
    }

}