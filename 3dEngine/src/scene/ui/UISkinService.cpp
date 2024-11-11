#include <stdexcept>
#include <memory>

#include <scene/ui/UISkinService.h>
#include <scene/ui/widget/LengthType.h>
#include <resources/ResourceRetriever.h>
#include <resources/image/Image.h>

namespace urchin {

    UISkinService& UISkinService::instance() {
        static UISkinService instance;
        return instance;
    }

    UISkinService::UISkinService() :
            skinReader(nullptr) {

    }

    void UISkinService::setSkin(const std::string& skinFilename) {
        skinReader = std::make_unique<UdaParser>(FileSystem::instance().getResourcesDirectory() + skinFilename, UdaLoadType::LOAD_FILE);
    }

    std::shared_ptr<Texture> UISkinService::createWidgetTexture(unsigned int width, unsigned int height, const UdaChunk* skinChunk, WidgetOutline* widgetOutline) const {
        //skin information
        auto widgetImageElem = getSkinReader().getFirstChunk(true, "image", UdaAttribute(), skinChunk);
        auto rawWidgetImage = ResourceRetriever::instance().getResource<Image>(widgetImageElem->getStringValue());

        auto topElem = getSkinReader().getFirstChunk(true, "part", UdaAttribute("zone", "top"), skinChunk);
        unsigned int top = topElem->getUnsignedIntValue();

        auto bottomElem = getSkinReader().getFirstChunk(true, "part", UdaAttribute("zone", "bottom"), skinChunk);
        unsigned int bottom = bottomElem->getUnsignedIntValue();

        auto leftElem = getSkinReader().getFirstChunk(true, "part", UdaAttribute("zone", "left"), skinChunk);
        unsigned int left = leftElem->getUnsignedIntValue();

        auto rightElem = getSkinReader().getFirstChunk(true, "part", UdaAttribute("zone", "right"), skinChunk);
        unsigned int right = rightElem->getUnsignedIntValue();

        //copy the information into the outline
        if (widgetOutline) {
            widgetOutline->topWidth = (int)top;
            widgetOutline->bottomWidth = (int)bottom;
            widgetOutline->leftWidth = (int)left;
            widgetOutline->rightWidth = (int)right;
        }

        //creates the image width * height
        unsigned int componentsCount = rawWidgetImage->retrieveComponentsCount();
        const std::vector<unsigned char>& rawTexels = rawWidgetImage->getTexels();
        unsigned int rawWidth = rawWidgetImage->getWidth();
        unsigned int rawHeight = rawWidgetImage->getHeight();

        std::vector<unsigned char> texels(height * width * componentsCount, 0);

        auto widthMinusRight = (unsigned int)std::max(0, (int)width - (int)right);
        auto heightMinusBottom = (unsigned int)std::max(0, (int)height - (int)bottom);
        unsigned int leftMultiplyInternalFormat = std::min(width, left * componentsCount);
        unsigned int topAdjusted = std::min(height, top);

        //copy corner top left
        for (unsigned int i = 0; i < topAdjusted; ++i) {
            for (unsigned int j = 0; j < leftMultiplyInternalFormat; ++j) {
                texels[i * width * componentsCount + j] = rawTexels[i * rawWidth * componentsCount + j];
            }
        }

        //copy top
        for (unsigned int i = 0; i < topAdjusted; ++i) {
            for (unsigned int j = left * componentsCount, k = 0; j < widthMinusRight * componentsCount; ++j, ++k) {
                texels[i * width * componentsCount + j] = rawTexels[i * rawWidth * componentsCount + k % ((rawWidth - (right + left)) * componentsCount) + left * componentsCount];
            }
        }

        //copy corner top right
        for (unsigned int i = 0; i < topAdjusted; ++i) {
            for (unsigned int j = widthMinusRight * componentsCount, k = (rawWidth - right) * componentsCount; j < width * componentsCount; ++j, ++k) {
                texels[i * width * componentsCount + j] = rawTexels[i * rawWidth * componentsCount + k];
            }
        }

        //copy right
        for (unsigned int i = top, k = 0; i < heightMinusBottom; ++i, ++k) {
            for (unsigned int j = widthMinusRight * componentsCount, l = (rawWidth - right) * componentsCount; j < width * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawTexels[(k % (rawHeight - (top + bottom)) + top) * rawWidth * componentsCount + l];
            }
        }

        //copy corner bottom right
        for (unsigned int i = heightMinusBottom, k = rawHeight - bottom; i < height; ++i, ++k) {
            for (unsigned int j = widthMinusRight * componentsCount, l = (rawWidth - right) * componentsCount; j < width * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawTexels[k * rawWidth * componentsCount + l];
            }
        }

        //copy bottom
        for (unsigned int i = heightMinusBottom, k = rawHeight - bottom; i < height; ++i, ++k) {
            for (unsigned int j = left * componentsCount, l = 0; j < widthMinusRight * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawTexels[k * rawWidth * componentsCount + l % ((rawWidth - (left + right)) * componentsCount) + left * componentsCount];
            }
        }

        //copy corner bottom left
        for (unsigned int i = heightMinusBottom, k = 0; i < height; ++i, ++k) {
            for (unsigned int j = 0; j < leftMultiplyInternalFormat; ++j) {
                texels[i * width * componentsCount + j] = rawTexels[(k + rawHeight - bottom) * rawWidth * componentsCount + j];
            }
        }

        //copy left
        for (unsigned int i = top, k = 0;i < heightMinusBottom;++i, ++k) {
            for (unsigned int j = 0; j < leftMultiplyInternalFormat; ++j) {
                texels[i * width * componentsCount + j] = rawTexels[(k % (rawHeight - (top + bottom)) + top) * rawWidth * componentsCount + j];
            }
        }

        //copy middle
        for (unsigned int i = top, k = 0; i < heightMinusBottom; ++i, ++k) {
            for (unsigned int j = leftMultiplyInternalFormat, l = 0; j < widthMinusRight * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawTexels[(k % (rawHeight - (top + bottom)) + top) * rawWidth * componentsCount + l % ((rawWidth - (left + right)) * componentsCount) + left * componentsCount];
            }
        }

        //create the texture
        return Texture::build("9-slicing widget", width, height, rawWidgetImage->retrieveTextureFormat(), texels.data(), TextureDataType::INT_8);
    }

    Length UISkinService::loadLength(const UdaChunk* mainChunk, std::string_view lengthName) const {
        auto lengthChunk = getSkinReader().getFirstChunk(true, lengthName, UdaAttribute(), mainChunk);
        float value = getSkinReader().getFirstChunk(true, "value", UdaAttribute(), lengthChunk)->getFloatValue();

        std::string lengthTypeString = getSkinReader().getFirstChunk(true, "type", UdaAttribute(), lengthChunk)->getStringValue();
        if (StringUtil::insensitiveEquals(lengthTypeString, "pixel")) {
            return Length{.value = value, .type = PIXEL};
        } else if (StringUtil::insensitiveEquals(lengthTypeString, "screenPercent")) {
            return Length{.value = value, .type = SCREEN_PERCENT};
        } else if (StringUtil::insensitiveEquals(lengthTypeString, "containerPercent")) {
            return Length{.value = value, .type = CONTAINER_PERCENT};
        } else if (StringUtil::insensitiveEquals(lengthTypeString, "parentPercent")) {
            return Length{.value = value, .type = PARENT_PERCENT};
        }

        throw std::runtime_error("Unknown length type: " + lengthTypeString);
    }

    const UdaParser& UISkinService::getSkinReader() const {
        if (!skinReader) {
            throw std::runtime_error("UI skin is not initialized");
        }
        return *skinReader;
    }

}
