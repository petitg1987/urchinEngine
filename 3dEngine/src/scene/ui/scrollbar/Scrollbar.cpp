#include <scene/ui/scrollbar/Scrollbar.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    Scrollbar::Scrollbar(Widget& scrollableWidget, const std::string& skinName) :
            scrollableWidget(scrollableWidget),
            skinName(skinName) {

    }

    void Scrollbar::initializeOrUpdate() {
        //detach children
        if (scrollbarCursor && scrollbarCursor->getParent() == &scrollableWidget) {
            scrollableWidget.detachChild(scrollbarCursor.get());
            scrollableWidget.detachChild(scrollbarLine.get());
        }

        //skin information
        auto scrollbarChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "scrollbar", UdaAttribute("skin", skinName));

        auto cursorImageChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "imageCursor", UdaAttribute(), scrollbarChunk);
        std::string cursorImageFilename = cursorImageChunk->getStringValue();
        Length scrollbarWidth = UISkinService::instance().loadLength(scrollbarChunk, "width");
        auto imageCursor = loadTexture(scrollbarChunk, "imageCursor");
        auto cursorImageRatio = (float)imageCursor->getHeight() / (float)imageCursor->getWidth();
        auto cursorWidthInPixel = (float)scrollableWidget.widthInPixel(scrollbarWidth.getValue(), scrollbarWidth.getType(), [](){return 0.0f;});

        auto lineImageChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "imageLine", UdaAttribute(), scrollbarChunk);
        std::string lineImageFilename = lineImageChunk->getStringValue();

        scrollbarLine = StaticBitmap::newStaticBitmap(&scrollableWidget, Position((float)scrollableWidget.getWidth() - cursorWidthInPixel, 0.0f, LengthType::PIXEL), Size(scrollbarWidth.getValue(), scrollbarWidth.getType(), 100.0f, LengthType::CONTAINER_PERCENT), lineImageFilename);
        scrollbarCursor = StaticBitmap::newStaticBitmap(&scrollableWidget, Position((float)scrollableWidget.getWidth() - cursorWidthInPixel, 0.0f, LengthType::PIXEL), Size(scrollbarWidth.getValue(), scrollbarWidth.getType(), cursorImageRatio, LengthType::RELATIVE_LENGTH), cursorImageFilename);
    }

    bool Scrollbar::onKeyPressEvent(unsigned int) {
        return false;
    }

    bool Scrollbar::onKeyReleaseEvent(unsigned int) {
        return false;
    }

    bool Scrollbar::onMouseMoveEvent(int, int) {
        return false;
    }

    std::shared_ptr<Texture> Scrollbar::loadTexture(const UdaChunk* scrollbarChunk, const std::string& chunkName) const {
        auto imageElem = UISkinService::instance().getSkinReader().getUniqueChunk(true, chunkName, UdaAttribute(), scrollbarChunk);
        auto img = ResourceRetriever::instance().getResource<Image>(imageElem->getStringValue());
        return img->createTexture(false);
    }

}