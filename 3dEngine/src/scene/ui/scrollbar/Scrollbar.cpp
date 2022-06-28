#include <utility>

#include <scene/ui/scrollbar/Scrollbar.h>
#include <scene/ui/scrollbar/Scrollable.h>
#include <resources/ResourceRetriever.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Scrollbar::Scrollbar(Widget& scrollableWidget, std::string skinName) :
            SCROLL_SPEED(ConfigService::instance().getFloatValue("ui.scrollSpeed")),
            scrollableWidget(scrollableWidget),
            skinName(std::move(skinName)),
            visibleHeight(0),
            contentHeight(0),
            mouseX(0),
            mouseY(0),
            state(DEFAULT),
            scrollPercentage(0.0f),
            shiftPixelPositionY(0) {

    }

    void Scrollbar::initializeOrUpdate() {
        //detach children
        if (scrollbarCursor && scrollbarCursor->getParent() == &scrollableWidget) {
            scrollableWidget.detachChild(scrollbarCursor.get());
            scrollableWidget.detachChild(scrollbarLine.get());
        }

        //skin information
        auto scrollbarChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "scrollbar", UdaAttribute("skin", skinName));

        auto cursorImageChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "imageCursor", UdaAttribute(), scrollbarChunk);
        std::string cursorImageFilename = cursorImageChunk->getStringValue();
        Length scrollbarWidth = UISkinService::instance().loadLength(scrollbarChunk, "width");
        auto imageCursor = loadTexture(scrollbarChunk, "imageCursor");
        auto cursorImageRatio = (float)imageCursor->getHeight() / (float)imageCursor->getWidth();
        float cursorWidthInPixel = scrollableWidget.widthLengthToPixel(scrollbarWidth.value, scrollbarWidth.type, [](){return 0.0f;});

        auto lineImageChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "imageLine", UdaAttribute(), scrollbarChunk);
        std::string lineImageFilename = lineImageChunk->getStringValue();
        auto imageLine = loadTexture(scrollbarChunk, "imageLine");
        if (imageCursor->getWidth() != imageLine->getWidth()) {
            throw std::runtime_error("Cursor and line images must have the same width");
        }

        scrollbarLine = StaticBitmap::create(&scrollableWidget, Position(scrollableWidget.getWidth() - cursorWidthInPixel, 0.0f, LengthType::PIXEL),
                                             Size(scrollbarWidth.value, scrollbarWidth.type, scrollableWidget.getHeight(), LengthType::PIXEL), lineImageFilename);
        scrollbarCursor = StaticBitmap::create(&scrollableWidget, Position(scrollableWidget.getWidth() - cursorWidthInPixel, 0.0f, LengthType::PIXEL),
                                               Size(scrollbarWidth.value, scrollbarWidth.type, cursorImageRatio, LengthType::RATIO_TO_WIDTH), cursorImageFilename);

        //update scrollbar
        onScrollableWidgetsUpdated();
    }

    void Scrollbar::onScrollableWidgetsUpdated() {
        if (scrollableWidget.isInitialized()) {
            //compute values
            float minChildPositionY = std::numeric_limits<float>::max();
            float maxChildPositionY = 0.0f;
            for (Widget* contentChild : getContentChildren()) {
                if (contentChild->isInitialized()) {
                    float childMinPositionY = contentChild->getGlobalPositionY();
                    if (childMinPositionY < minChildPositionY) {
                        minChildPositionY = childMinPositionY;
                    }

                    float childMaxPositionY = childMinPositionY + contentChild->getHeight();
                    if (childMaxPositionY > maxChildPositionY) {
                        maxChildPositionY = childMaxPositionY;
                    }
                }
            }

            if (minChildPositionY < maxChildPositionY) {
                contentHeight = maxChildPositionY - minChildPositionY;
            } else {
                contentHeight = 0.0f;
            }
            visibleHeight = scrollableWidget.getHeight();

            updateScrollingPosition();
        }
    }

    bool Scrollbar::onKeyPressEvent(unsigned int key) {
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
            if (scrollbarCursor->widgetRectangle().collideWithPoint(Point2<int>(mouseX, mouseY))) {
                state = CURSOR_SELECTED;
            } else if (scrollbarLine->widgetRectangle().collideWithPoint(Point2<int>(mouseX, mouseY))) {
                updateScrollingPosition(mouseY);
                state = CURSOR_SELECTED;
            }
        }
        return true;
    }

    bool Scrollbar::onKeyReleaseEvent(unsigned int key) {
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
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
        if (scrollableWidget.getWidgetState() == Widget::WidgetState::FOCUS && isScrollbarRequired()) {
            float scrollMoveSpeedFactor = visibleHeight / (contentHeight - visibleHeight);
            float deltaScroll = (float) offsetY * SCROLL_SPEED * scrollMoveSpeedFactor;
            scrollPercentage -= deltaScroll;
            scrollPercentage = std::clamp(scrollPercentage, 0.0f, 1.0f);

            updateScrollingPosition();
            return false;
        }
        return true;
    }

    bool Scrollbar::isScrollbarWidget(const Widget* widget) const {
        return widget == scrollbarCursor.get() || widget == scrollbarLine.get();
    }

    bool Scrollbar::isScrollbarRequired() const {
        return contentHeight > visibleHeight + 0.001f;
    }

    int Scrollbar::getScrollShiftY() const {
        return shiftPixelPositionY;
    }

    std::shared_ptr<Texture> Scrollbar::loadTexture(const UdaChunk* scrollbarChunk, std::string_view chunkName) const {
        auto imageElem = UISkinService::instance().getSkinReader().getFirstChunk(true, chunkName, UdaAttribute(), scrollbarChunk);
        return ResourceRetriever::instance().getResource<Texture>(imageElem->getStringValue(), {{"mipMap", "0"}});
    }

    void Scrollbar::updateScrollingPosition(int positionY) {
        auto halfCursorHeight = scrollbarCursor->getHeight() / 2.0f;
        auto minPositionY = scrollbarLine->getGlobalPositionY() + halfCursorHeight;
        auto maxPositionY = scrollbarLine->getGlobalPositionY() + scrollbarLine->getHeight() - halfCursorHeight;

        scrollPercentage = ((float)positionY - minPositionY) / (maxPositionY - minPositionY);
        scrollPercentage = std::clamp(scrollPercentage, 0.0f, 1.0f);

        updateScrollingPosition();
    }

    void Scrollbar::updateScrollingPosition() {
        if (scrollbarLine && scrollbarCursor) {
            scrollbarLine->setIsVisible(isScrollbarRequired());
            scrollbarCursor->setIsVisible(isScrollbarRequired());

            if (isScrollbarRequired()) {
                updateCursorPosition();
                computeShiftPositionY();
            } else if (!MathFunction::isZero(scrollPercentage)) {
                scrollPercentage = 0.0f;
                computeShiftPositionY();
            }
            dynamic_cast<Scrollable&>(this->scrollableWidget).onScrollableContentUpdated();
        }
    }

    void Scrollbar::updateCursorPosition() const {
        float cursorPositionX = scrollbarCursor->getPosition().getX();
        LengthType cursorPositionXType = scrollbarCursor->getPosition().getXType();

        float cursorMaxPositionY = scrollableWidget.getHeight() - scrollbarCursor->getHeight();
        float cursorPositionY = cursorMaxPositionY * scrollPercentage;

        scrollbarCursor->updatePosition(Position(cursorPositionX, cursorPositionXType, cursorPositionY, LengthType::PIXEL));
    }

    void Scrollbar::computeShiftPositionY() {
        shiftPixelPositionY = -(int)((contentHeight - visibleHeight) * scrollPercentage);

        //compensate the shift applied on all children (including scrollbarLine & scrollbarCursor)
        scrollbarLine->updatePosition(Position(scrollbarLine->getPosition().getX(), 0.0f - (float)shiftPixelPositionY, LengthType::PIXEL));
        scrollbarCursor->updatePosition(Position(scrollbarCursor->getPosition().getX(), scrollbarCursor->getPosition().getY() - (float)shiftPixelPositionY, LengthType::PIXEL));
    }

    std::vector<Widget*> Scrollbar::getContentChildren() const {
        std::vector<Widget*> contentChildren;
        contentChildren.reserve(scrollableWidget.getChildren().size());

        for (auto& child : scrollableWidget.getChildren()) {
            if (!isScrollbarWidget(child.get())){
                contentChildren.push_back(child.get());
            }
        }
        return contentChildren;
    }

}