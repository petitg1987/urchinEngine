#include <stdexcept>
#include <memory>

#include "scene/ui/UISkinService.h"
#include "scene/ui/NineSlicingTexture.h"
#include "scene/ui/widget/LengthType.h"
#include "resources/ResourceRetriever.h"
#include "resources/image/Image.h"

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

    std::shared_ptr<Texture> UISkinService::createWidgetTexture(unsigned int width, unsigned int height, const UdaChunk* skinChunk, Outline* widgetOutline) const {
        auto widgetImageElem = getSkinReader().getFirstChunk(true, "image", UdaAttribute(), skinChunk);
        auto rawWidgetImage = ResourceRetriever::instance().getResource<Image>(widgetImageElem->getStringValue());

        Outline sourceOutline {
            .topPixel = loadSourceOutline(skinChunk, "top"),
            .bottomPixel = loadSourceOutline(skinChunk, "bottom"),
            .leftPixel = loadSourceOutline(skinChunk, "left"),
            .rightPixel = loadSourceOutline(skinChunk, "right")
        };

        Outline targetOutline {
            .topPixel = loadTargetOutline(skinChunk, "top", sourceOutline.topPixel, height),
            .bottomPixel = loadTargetOutline(skinChunk, "bottom", sourceOutline.bottomPixel, height),
            .leftPixel = loadTargetOutline(skinChunk, "left", sourceOutline.leftPixel, width),
            .rightPixel = loadTargetOutline(skinChunk, "right", sourceOutline.rightPixel, width)
        };

        if (widgetOutline) {
            *widgetOutline = targetOutline;
        }
        return NineSlicingTexture().build(rawWidgetImage, width, height, sourceOutline, targetOutline);
    }

    unsigned int UISkinService::loadSourceOutline(const UdaChunk* skinChunk, std::string zone) const {
        auto sourceElem = getSkinReader().getFirstChunk(true, "sourceOutlinePixel", UdaAttribute("zone", std::move(zone)), skinChunk);
        if (!sourceElem) {
            return 0;
        }
        return sourceElem->getUnsignedIntValue();
    }

    unsigned int UISkinService::loadTargetOutline(const UdaChunk* skinChunk, std::string zone, unsigned int sourceWidth, unsigned int widgetLength) const {
        auto targetPercentElem = getSkinReader().getFirstChunk(false, "outlineTargetPercent", UdaAttribute("zone", std::move(zone)), skinChunk);
        if (!targetPercentElem) {
            return sourceWidth;
        }

        float targetPercent = targetPercentElem->getFloatValue();
        if (targetPercent < 0.0f || targetPercent > 100.0f) {
            throw std::runtime_error("Target percent of zone " + std::string(zone) + " must be between 0 and 100: " + std::to_string(targetPercent));
        }
        return MathFunction::roundToUInt(targetPercent / 100.0f * (float)widgetLength);
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
