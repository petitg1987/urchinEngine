#include <scene/ui/widget/container/Container.h>

namespace urchin {

    Container::Container(Position position, Size size) : //TODO disable event outside the container ("> Load" button is clickable even when not visible)
            Widget(position, size),
            scissorEnabled(true) {

    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position) {
        return create<Container>(new Container(position, Size(100.0f, 100.0f, LengthType::SCREEN_PERCENT)), parent);
    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position, Size size) {
        return create<Container>(new Container(position, size), parent);
    }

    void Container::addChild(const std::shared_ptr<Widget>& child) {
        Widget::addChild(child);
        if (isScrollbarEnabled()) {
            scrollbar->onScrollableWidgetContentUpdated();
        }
    }

    void Container::detachChild(Widget* child){
        Widget::detachChild(child);
        if (isScrollbarEnabled()) {
            scrollbar->onScrollableWidgetContentUpdated();
        }
    }

    void Container::detachChildren() {
        Widget::detachChildren();
        if (isScrollbarEnabled()) {
            scrollbar->onScrollableWidgetContentUpdated();
        }
    }

    void Container::resetChildren() {
        detachChildren();

        //add scrollbar children
        if (isScrollbarEnabled()) {
            scrollbar->initializeOrUpdate();
        }
    }

    void Container::enableScissor(bool scissorEnabled) {
        this->scissorEnabled = scissorEnabled;
    }

    bool Container::isScissorEnabled() const {
        return scissorEnabled;
    }

    void Container::enableScrollbar(bool scrollbarEnabled, const std::string& scrollbarSkinName) {
        if (scrollbarEnabled) {
            scrollbar = std::make_unique<Scrollbar>(*this, scrollbarSkinName);
        } else {
            scrollbar.reset();
        }
    }

    bool Container::isScrollbarEnabled() const {
        return scrollbar != nullptr;
    }

    void Container::createOrUpdateWidget() {
        if (isScrollbarEnabled()) {
            scrollbar->initializeOrUpdate();
        }
    }

    bool Container::onKeyPressEvent(unsigned int key) {
        if (isScrollbarEnabled()) {
            return scrollbar->onKeyPressEvent(key);
        }
        return true;
    }

    bool Container::onKeyReleaseEvent(unsigned int key) {
        if (isScrollbarEnabled()) {
            return scrollbar->onKeyReleaseEvent(key);
        }
        return true;
    }

    bool Container::onMouseMoveEvent(int mouseX, int mouseY) {
        if (isScrollbarEnabled()) {
            return scrollbar->onMouseMoveEvent(mouseX, mouseY);
        }
        return true;
    }

    bool Container::onScrollEvent(double offsetY) {
        if (isScrollbarEnabled()) {
            return scrollbar->onScrollEvent(offsetY);
        }
        return true;
    }

    void Container::prepareWidgetRendering(float) {

    }

}
