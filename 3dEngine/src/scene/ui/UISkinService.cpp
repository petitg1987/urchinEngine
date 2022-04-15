#include <stdexcept>
#include <memory>

#include <scene/ui/UISkinService.h>
#include <scene/ui/widget/LengthType.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    UISkinService::UISkinService() :
            Singleton<UISkinService>(),
            skinReader(nullptr) {

    }

    void UISkinService::setSkin(const std::string& skinFilename) {
        skinReader = std::make_unique<UdaParser>(FileSystem::instance().getResourcesDirectory() + skinFilename);
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
        unsigned int componentsCount = rawWidgetImage->retrieveComponentsCount(); //shortcut
        std::vector<unsigned char> texels(height * width * componentsCount, 0);

        auto widthMinusRight = (unsigned int)std::max(0, (int)width - (int)right);
        auto heightMinusBottom = (unsigned int)std::max(0, (int)height - (int)bottom);
        unsigned int leftMultiplyInternalFormat = std::min(width, left * componentsCount);
        unsigned int topAdjusted = std::min(height, top);

        //copy corner top left
        for (unsigned int i = 0; i < topAdjusted; ++i) {
            for (unsigned int j = 0; j < leftMultiplyInternalFormat; ++j) {
                texels[i * width * componentsCount + j] = rawWidgetImage->getTexels()[i * rawWidgetImage->getWidth() * componentsCount + j];
            }
        }

        //copy top
        for (unsigned int i = 0; i < topAdjusted; ++i) {
            for (unsigned int j = left * componentsCount, k = 0; j < widthMinusRight * componentsCount; ++j, ++k) {
                texels[i * width * componentsCount + j] = rawWidgetImage->getTexels()[i * rawWidgetImage->getWidth() * componentsCount + k % ((rawWidgetImage->getWidth() - (right + left)) * componentsCount) + left * componentsCount];
            }
        }

        //copy corner top right
        for (unsigned int i = 0; i < topAdjusted; ++i) {
            for (unsigned int j = widthMinusRight * componentsCount, k = (rawWidgetImage->getWidth()-right) * componentsCount; j < width * componentsCount; ++j, ++k) {
                texels[i * width * componentsCount + j] = rawWidgetImage->getTexels()[i * rawWidgetImage->getWidth() * componentsCount + k];
            }
        }

        //copy right
        for (unsigned int i = top, k = 0; i < heightMinusBottom; ++i, ++k) {
            for (unsigned int j = widthMinusRight * componentsCount, l = (rawWidgetImage->getWidth() - right) * componentsCount; j < width * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawWidgetImage->getTexels()[(k % (rawWidgetImage->getHeight() - (top + bottom)) + top) * rawWidgetImage->getWidth() * componentsCount + l];
            }
        }

        //copy corner bottom right
        for (unsigned int i = heightMinusBottom, k = rawWidgetImage->getHeight() - bottom; i < height; ++i, ++k) {
            for (unsigned int j = widthMinusRight * componentsCount, l = (rawWidgetImage->getWidth() - right) * componentsCount; j < width * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawWidgetImage->getTexels()[k * rawWidgetImage->getWidth() * componentsCount + l];
            }
        }

        //copy bottom
        for (unsigned int i = heightMinusBottom, k = rawWidgetImage->getHeight() - bottom; i < height; ++i, ++k) {
            for (unsigned int j = left * componentsCount, l = 0; j < widthMinusRight * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawWidgetImage->getTexels()[k * rawWidgetImage->getWidth() * componentsCount + l % ((rawWidgetImage->getWidth() - (left + right)) * componentsCount) + left * componentsCount];
            }
        }

        //copy corner bottom left
        for (unsigned int i = heightMinusBottom, k = 0; i < height; ++i, ++k) {
            for (unsigned int j = 0; j < leftMultiplyInternalFormat; ++j) {
                texels[i * width * componentsCount + j] = rawWidgetImage->getTexels()[(k + rawWidgetImage->getHeight() - bottom) * rawWidgetImage->getWidth() * componentsCount + j];
            }
        }

        //copy left
        for (unsigned int i = top, k = 0;i < heightMinusBottom;++i, ++k) {
            for (unsigned int j = 0; j < leftMultiplyInternalFormat; ++j) {
                texels[i * width * componentsCount + j] = rawWidgetImage->getTexels()[(k % (rawWidgetImage->getHeight() - (top + bottom)) + top) * rawWidgetImage->getWidth() * componentsCount + j];
            }
        }

        //copy middle
        for (unsigned int i = top, k = 0; i < heightMinusBottom; ++i, ++k) {
            for (unsigned int j = leftMultiplyInternalFormat, l = 0; j < widthMinusRight * componentsCount; ++j, ++l) {
                texels[i * width * componentsCount + j] = rawWidgetImage->getTexels()[(k % (rawWidgetImage->getHeight() - (top + bottom)) + top) * rawWidgetImage->getWidth() * componentsCount + l%((rawWidgetImage->getWidth() - (left + right)) * componentsCount) + left * componentsCount];
            }
        }

        //create the texture
        return Texture::build("9-slicing widget", width, height, rawWidgetImage->retrieveTextureFormat(), texels.data());
    }

    /**
     * @param lengthType [out] Type of the length
     */
    float UISkinService::loadLength(const UdaChunk* mainChunk, std::string_view lengthName, LengthType& lengthType) const {
        auto lengthChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, lengthName, UdaAttribute(), mainChunk);

        std::string lengthTypeString = UISkinService::instance().getSkinReader().getFirstChunk(true, "type", UdaAttribute(), lengthChunk)->getStringValue();
        if (StringUtil::insensitiveEquals(lengthTypeString, "pixel")) {
            lengthType = PIXEL;
        } else if (StringUtil::insensitiveEquals(lengthTypeString, "screenPercent")) {
            lengthType = SCREEN_PERCENT;
        } else if (StringUtil::insensitiveEquals(lengthTypeString, "containerPercent")) {
            lengthType = CONTAINER_PERCENT;
        } else if (StringUtil::insensitiveEquals(lengthTypeString, "parentPercent")) {
            lengthType = PARENT_PERCENT;
        } else {
            throw std::runtime_error("Unknown length type: " + lengthTypeString);
        }

        return UISkinService::instance().getSkinReader().getFirstChunk(true, "value", UdaAttribute(), lengthChunk)->getFloatValue();
    }

    const UdaParser& UISkinService::getSkinReader() const {
        if (!skinReader) {
            throw std::runtime_error("UI skin is not initialized");
        }
        return *skinReader;
    }

}
