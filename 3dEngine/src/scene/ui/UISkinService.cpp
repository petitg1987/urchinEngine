#include <stdexcept>
#include <memory>

#include <scene/ui/UISkinService.h>
#include <scene/ui/widget/LengthType.h>
#include <resources/MediaManager.h>

namespace urchin {

    UISkinService::UISkinService() :
            Singleton<UISkinService>(),
            skinReader(nullptr) {

    }

    void UISkinService::setSkin(const std::string& skinFilename) {
        skinReader = std::make_unique<XmlParser>(skinFilename);
    }

    std::shared_ptr<Texture> UISkinService::createWidgetTexture(unsigned int width, unsigned int height, const XmlChunk* skinXmlChunk, WidgetOutline* widgetOutline) const {
        //skin information
        auto widgetImageElem = getSkinReader()->getUniqueChunk(true, "image", XmlAttribute(), skinXmlChunk);
        auto* rawWidgetImage = MediaManager::instance()->getMedia<Image>(widgetImageElem->getStringValue());

        auto topElem = getSkinReader()->getUniqueChunk(true, "part", XmlAttribute("zone", "top"), skinXmlChunk);
        unsigned int top = topElem->getUnsignedIntValue();

        auto bottomElem = getSkinReader()->getUniqueChunk(true, "part", XmlAttribute("zone", "bottom"), skinXmlChunk);
        unsigned int bottom = bottomElem->getUnsignedIntValue();

        auto leftElem = getSkinReader()->getUniqueChunk(true, "part", XmlAttribute("zone", "left"), skinXmlChunk);
        unsigned int left = leftElem->getUnsignedIntValue();

        auto rightElem = getSkinReader()->getUniqueChunk(true, "part", XmlAttribute("zone", "right"), skinXmlChunk);
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

        unsigned int widthMinusRight = (unsigned int)std::max(0, (int)width - (int)right);
        unsigned int heightMinusBottom = (unsigned int)std::max(0, (int)height - (int)bottom);
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
        auto widgetTexture = Image(width, height, rawWidgetImage->getImageFormat(), std::move(texels)).createTexture(false);

        rawWidgetImage->release();
        return widgetTexture;
    }

    Length UISkinService::loadLength(const XmlChunk* mainChunk, const std::string& lengthName) const {
        auto fontHeightChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, lengthName, XmlAttribute(), mainChunk);

        float length = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "value", XmlAttribute(), fontHeightChunk.get())->getFloatValue();

        const std::string& lengthTypeString = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "type", XmlAttribute(), fontHeightChunk.get())->getStringValue();
        if (StringUtil::insensitiveEquals(lengthTypeString, "pixel")) {
            return Length(length, LengthType::PIXEL);
        } else if (StringUtil::insensitiveEquals(lengthTypeString, "percentage")) {
            return Length(length, LengthType::PERCENTAGE);
        }
        throw std::runtime_error("Unknown length type: " + lengthTypeString);
    }

    const std::unique_ptr<XmlParser>& UISkinService::getSkinReader() const {
        if (!skinReader) {
            throw std::runtime_error("UI skin is not initialized");
        }
        return skinReader;
    }

}
