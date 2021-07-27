#include <scene/ui/widget/container/Container.h>

namespace urchin {

    Container::Container(Position position, Size size, std::string skinName) : //TODO disable event outside the container ("> Load" button is clickable even when not visible)
            Widget(position, size),
            skinName(std::move(skinName)),
            scissorEnabled(true) {
        scrollbar = std::make_unique<Scrollbar>(*this, this->skinName);
    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position, std::string skinName) { //TODO remove this method
        return create<Container>(new Container(position, Size(100.0f, 100.0f, LengthType::SCREEN_PERCENT), std::move(skinName)), parent);
    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position, Size size, std::string skinName) {
        return create<Container>(new Container(position, size, std::move(skinName)), parent);
    }

    void Container::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        Widget::onResize(sceneWidth, sceneHeight);
        scrollbar->onScrollableWidgetsUpdated();
    }

    void Container::addChild(const std::shared_ptr<Widget>& child) {
        Widget::addChild(child);
        scrollbar->onScrollableWidgetsUpdated();
    }

    void Container::detachChild(Widget* child){
        Widget::detachChild(child);
        scrollbar->onScrollableWidgetsUpdated();
    }

    void Container::detachChildren() {
        Widget::detachChildren();
        scrollbar->onScrollableWidgetsUpdated();
    }

    void Container::resetChildren() {
        detachChildren();

        //re-add scrollbar children
        scrollbar->initializeOrUpdate();
    }

    void Container::enableScissor(bool scissorEnabled) {
        this->scissorEnabled = scissorEnabled;
    }

    bool Container::isScissorEnabled() const {
        return scissorEnabled;
    }

    int Container::getScrollShiftY() const {
        if (scrollbar) {
            return scrollbar->getScrollShiftY();
        }
        return 0;
    }

    void Container::createOrUpdateWidget() {
        scrollbar->initializeOrUpdate();
    }

    bool Container::onKeyPressEvent(unsigned int key) {
        return scrollbar->onKeyPressEvent(key);
    }

    bool Container::onKeyReleaseEvent(unsigned int key) {
        return scrollbar->onKeyReleaseEvent(key);
    }

    bool Container::onMouseMoveEvent(int mouseX, int mouseY) {
        return scrollbar->onMouseMoveEvent(mouseX, mouseY);
    }

    bool Container::onScrollEvent(double offsetY) {
        return scrollbar->onScrollEvent(offsetY);
    }

    void Container::prepareWidgetRendering(float) {

    }

}
