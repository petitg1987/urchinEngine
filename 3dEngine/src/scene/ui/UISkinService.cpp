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

        auto topElem = getSkinReader().getFirstChunk(true, "part", UdaAttribute("zone", "top"), skinChunk);
        auto bottomElem = getSkinReader().getFirstChunk(true, "part", UdaAttribute("zone", "bottom"), skinChunk);
        auto leftElem = getSkinReader().getFirstChunk(true, "part", UdaAttribute("zone", "left"), skinChunk);
        auto rightElem = getSkinReader().getFirstChunk(true, "part", UdaAttribute("zone", "right"), skinChunk);

        Outline outline {
            .topWidth = topElem->getUnsignedIntValue(),
            .bottomWidth = bottomElem->getUnsignedIntValue(),
            .leftWidth = leftElem->getUnsignedIntValue(),
            .rightWidth = rightElem->getUnsignedIntValue()
        };

        if (widgetOutline) {
            *widgetOutline = outline;
        }
        return NineSlicingTexture().build(rawWidgetImage, width, height, outline);
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
