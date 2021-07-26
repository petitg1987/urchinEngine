#include <scene/ui/widget/container/Container.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    Container::Container(Position position, Size size) :
            Widget(position, size),
            scissorEnabled(true),
            scrollbarEnabled(false) {

    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position) {
        return create<Container>(new Container(position, Size(100.0f, 100.0f, LengthType::SCREEN_PERCENT)), parent);
    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position, Size size) {
        return create<Container>(new Container(position, size), parent);
    }

    void Container::resetChildren() {
        detachChildren();
        if (scrollbarEnabled) {
            addChild(scrollbarLine);
            addChild(scrollbarCursor);
        }
    }

    void Container::enableScissor(bool scissorEnabled) {
        this->scissorEnabled = scissorEnabled;
    }

    bool Container::isScissorEnabled() const {
        return scissorEnabled;
    }

    void Container::enableScrollbar(bool scrollbarEnabled, const std::string& scrollbarSkinName) {
        this->scrollbarEnabled = scrollbarEnabled;
        this->scrollbarSkinName = scrollbarSkinName;
    }

    void Container::createOrUpdateWidget() {
        if (scrollbarEnabled) {
            //detach children
            //TODO ...

            //skin information
            auto scrollbarChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "scrollbar", UdaAttribute("skin", scrollbarSkinName));

            auto cursorImageChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "imageCursor", UdaAttribute(), scrollbarChunk);
            std::string cursorImageFilename = cursorImageChunk->getStringValue();
            Length scrollbarWidth = UISkinService::instance().loadLength(scrollbarChunk, "width");
            auto imageCursor = loadTexture(scrollbarChunk, "imageCursor");
            auto cursorImageRatio = (float)imageCursor->getHeight() / (float)imageCursor->getWidth();
            auto cursorWidthInPixel = (float)widthInPixel(scrollbarWidth.getValue(), scrollbarWidth.getType(), [](){return 0.0f;});

            auto lineImageChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "imageLine", UdaAttribute(), scrollbarChunk);
            std::string lineImageFilename = lineImageChunk->getStringValue();

            //TODO review Size X, Y + Position X
            scrollbarLine = StaticBitmap::newStaticBitmap(this, Position((float)getWidth() - cursorWidthInPixel / 2.0f, 0.0f, LengthType::PIXEL), Size(5.0f, LengthType::PIXEL, 100.0f, LengthType::SCREEN_PERCENT), lineImageFilename);
            scrollbarCursor = StaticBitmap::newStaticBitmap(this, Position((float)getWidth() - cursorWidthInPixel, 0.0f, LengthType::PIXEL), Size(scrollbarWidth.getValue(), scrollbarWidth.getType(), cursorImageRatio, LengthType::RELATIVE_LENGTH), cursorImageFilename);
        }
    }

    void Container::prepareWidgetRendering(float) {

    }

    std::shared_ptr<Texture> Container::loadTexture(const UdaChunk* scrollbarChunk, const std::string& chunkName) const {
        auto imageElem = UISkinService::instance().getSkinReader().getUniqueChunk(true, chunkName, UdaAttribute(), scrollbarChunk);
        auto img = ResourceRetriever::instance().getResource<Image>(imageElem->getStringValue());
        return img->createTexture(false);
    }

}
