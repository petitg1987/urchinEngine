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

    void Container::resetChildren() {
        detachChildren();

        //add scrollbar children
        if (scrollbar) {
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

    void Container::createOrUpdateWidget() {
        if (scrollbar) {
            scrollbar->initializeOrUpdate();
        }
    }

    bool Container::onKeyPressEvent(unsigned int key) {
        if (scrollbar) {
            return scrollbar->onKeyPressEvent(key);
        }
        return true;
    }

    bool Container::onKeyReleaseEvent(unsigned int key) {
        if (scrollbar) {
            return scrollbar->onKeyReleaseEvent(key);
        }
        return true;
    }

    bool Container::onMouseMoveEvent(int mouseX, int mouseY) {
        if (scrollbar) {
            return scrollbar->onMouseMoveEvent(mouseX, mouseY);
        }
        return true;
    }

    void Container::prepareWidgetRendering(float) {

    }

}
