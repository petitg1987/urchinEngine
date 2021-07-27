#include <utility>

#include <scene/ui/scrollbar/Scrollbar.h>
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
            scrollPercentage(0.0f) {

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

        //size computation
        onScrollableWidgetContentUpdated();
    }

    void Scrollbar::onScrollableWidgetContentUpdated() {
        visibleHeight = (float)scrollableWidget.getHeight();

        float minChildPositionY = std::numeric_limits<float>::max();
        float maxChildPositionY = 0.0f;
        for (auto& contentChild : getContentChildren()) {
            auto childMinPositionY = (float)contentChild->getGlobalPositionY();
            if (childMinPositionY < minChildPositionY) {
                minChildPositionY = childMinPositionY;
            }

            auto childMaxPositionY = (float)contentChild->getGlobalPositionY() + (float)contentChild->getHeight();
            if (childMaxPositionY > maxChildPositionY) {
                maxChildPositionY = childMaxPositionY;
            }
        }
        contentHeight = maxChildPositionY - minChildPositionY;

        childrenOriginalPositions.clear();
        for (auto &contentChild : getContentChildren()) {
            childrenOriginalPositions.emplace(contentChild, contentChild->getPositionY());
        }
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
            float scrollMoveSpeedFactor = (contentHeight <= visibleHeight) ? 0.0f : (visibleHeight / contentHeight);
            float deltaScroll = (float)offsetY * SCROLL_SPEED * scrollMoveSpeedFactor;
            scrollPercentage -= deltaScroll;
            scrollPercentage = MathFunction::clamp(scrollPercentage, 0.0f, 1.0f);

            updateScrollingPosition();
        }
        return true;
    }

    std::shared_ptr<Texture> Scrollbar::loadTexture(const UdaChunk* scrollbarChunk, const std::string& chunkName) const {
        auto imageElem = UISkinService::instance().getSkinReader().getUniqueChunk(true, chunkName, UdaAttribute(), scrollbarChunk);
        auto img = ResourceRetriever::instance().getResource<Image>(imageElem->getStringValue());
        return img->createTexture(false);
    }

    void Scrollbar::updateScrollingPosition(int positionY) {
        auto minPositionY = (float)scrollbarLine->getGlobalPositionY();
        auto maxPositionY = (float)scrollbarLine->getGlobalPositionY() + (float)scrollbarLine->getHeight();

        scrollPercentage = ((float)positionY - minPositionY) / (maxPositionY - minPositionY);
        scrollPercentage = MathFunction::clamp(scrollPercentage, 0.0f, 1.0f);

        updateScrollingPosition();
    }

    void Scrollbar::updateScrollingPosition() {
        if (contentHeight > visibleHeight) {
            updateCursorPosition();
            updateContentPosition();
        }
    }

    void Scrollbar::updateCursorPosition() { //TODO custom adjustment...
        float cursorPositionX = scrollbarCursor->getPosition().getPositionX();
        LengthType cursorPositionTypeX = scrollbarCursor->getPosition().getPositionTypeX();

        auto cursorMinPositionY = 0.0f;
        auto cursorMaxPositionY = (float)scrollableWidget.getHeight();

        float cursorPositionY = cursorMinPositionY + (cursorMaxPositionY - cursorMinPositionY) * scrollPercentage;
        scrollbarCursor->updatePosition(Position(cursorPositionX, cursorPositionTypeX, cursorPositionY, LengthType::PIXEL));
    }

    void Scrollbar::updateContentPosition() { //TODO custom adjustment...
        float shiftPositionY = (contentHeight - visibleHeight) * scrollPercentage;

        for (auto& contentChild : getContentChildren()) {
            auto itFind = childrenOriginalPositions.find(contentChild);
            if (itFind == childrenOriginalPositions.end()) {
                throw std::runtime_error("Unknown children found in a scrollable content");
            }

            float originalPositionY = itFind->second;
            float newPositionY = originalPositionY - shiftPositionY;
            contentChild->updatePosition(Position(contentChild->getPosition().getPositionX(), contentChild->getPosition().getPositionTypeX(), newPositionY, LengthType::PIXEL));
        }
    }

    std::vector<Widget*> Scrollbar::getContentChildren() const {
        std::vector<Widget*> contentChildren;
        contentChildren.reserve(scrollableWidget.getChildren().size());
        for (auto& child : scrollableWidget.getChildren()) {
            if (child != scrollbarCursor && child != scrollbarLine) {
                contentChildren.push_back(child.get());
            }
        }
        return contentChildren;
    }

}