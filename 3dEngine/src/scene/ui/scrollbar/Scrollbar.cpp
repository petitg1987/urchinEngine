#include <utility>

#include <scene/ui/scrollbar/Scrollbar.h>
#include <resources/ResourceRetriever.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Scrollbar::Scrollbar(Widget& scrollableWidget, std::string skinName) :
            SCROLL_SPEED(ConfigService::instance().getFloatValue("ui.scrollSpeed")),
            scrollableWidget(scrollableWidget),
            skinName(std::move(skinName)),
            mouseX(0),
            mouseY(0),
            state(DEFAULT) {

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
        auto imageLine = loadTexture(scrollbarChunk, "imageLine");
        if (imageCursor->getWidth() != imageLine->getWidth()) {
            throw std::runtime_error("Cursor and line images must have the same width");
        }

        scrollbarLine = StaticBitmap::newStaticBitmap(&scrollableWidget, Position((float)scrollableWidget.getWidth() - cursorWidthInPixel, 0.0f, LengthType::PIXEL), Size(scrollbarWidth.getValue(), scrollbarWidth.getType(), 100.0f, LengthType::CONTAINER_PERCENT), lineImageFilename);
        scrollbarCursor = StaticBitmap::newStaticBitmap(&scrollableWidget, Position((float)scrollableWidget.getWidth() - cursorWidthInPixel, 0.0f, LengthType::PIXEL), Size(scrollbarWidth.getValue(), scrollbarWidth.getType(), cursorImageRatio, LengthType::RELATIVE_LENGTH), cursorImageFilename);
    }

    bool Scrollbar::onKeyPressEvent(unsigned int key) {
        if (key == InputDeviceKey::MOUSE_LEFT) {
            Rectangle<int> cursorRectangle(Point2<int>(scrollbarCursor->getGlobalPositionX(), scrollbarCursor->getGlobalPositionY()),
                                           Point2<int>(scrollbarCursor->getGlobalPositionX() + (int)scrollbarCursor->getWidth(), scrollbarCursor->getGlobalPositionY() + (int)scrollbarCursor->getHeight()));
            if (cursorRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
                state = CURSOR_SELECTED;
            } else {
                Rectangle<int> scrollbarRectangle(Point2<int>(scrollbarLine->getGlobalPositionX(), scrollbarLine->getGlobalPositionY()),
                                                  Point2<int>(scrollbarLine->getGlobalPositionX() + (int)scrollbarLine->getWidth(), scrollbarLine->getGlobalPositionY() + (int)scrollbarLine->getHeight()));
                if (scrollbarRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
                    updateScrollingPosition(mouseY);
                    state = CURSOR_SELECTED;
                }
            }
        }
        return true;
    }

    bool Scrollbar::onKeyReleaseEvent(unsigned int key) {
        if (key == InputDeviceKey::MOUSE_LEFT) {
            state = DEFAULT;
        }
        return true;
    }

    bool Scrollbar::onMouseMoveEvent(int mouseX, int mouseY) {
        this->mouseX = mouseX;
        this->mouseY = mouseY;

        if (state == CURSOR_SELECTED) {
            updateScrollingPosition(mouseY);
            return false;
        }
        return true;
    }

    bool Scrollbar::onScrollEvent(double offsetY) {
        if (scrollableWidget.getWidgetState() == Widget::WidgetStates::FOCUS) {
            auto positionPercentage = (float)offsetY * SCROLL_SPEED; //TODO compute percentage
            updateScrollingPercentage(positionPercentage);
        }
        return true;
    }

    std::shared_ptr<Texture> Scrollbar::loadTexture(const UdaChunk* scrollbarChunk, const std::string& chunkName) const {
        auto imageElem = UISkinService::instance().getSkinReader().getUniqueChunk(true, chunkName, UdaAttribute(), scrollbarChunk);
        auto img = ResourceRetriever::instance().getResource<Image>(imageElem->getStringValue());
        return img->createTexture(false);
    }

    void Scrollbar::updateScrollingPosition(int positionY) {
        auto positionPercentage = (float)positionY; //TODO compute percentage
        updateScrollingPercentage(positionPercentage);
    }

    void Scrollbar::updateScrollingPercentage(float /*positionPercentage*/) {
        //TODO impl

        for (auto &child : scrollableWidget.getChildren()) {
            if (child == scrollbarCursor || child == scrollbarLine) {
                continue;
            }

            int posX = child->getPositionX();
            int posY = child->getPositionY() - 1;
            child->updatePosition(Position((float) posX, (float) posY, LengthType::PIXEL));
        }
    }

}