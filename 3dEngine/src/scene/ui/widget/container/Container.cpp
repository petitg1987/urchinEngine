#include <scene/ui/widget/container/Container.h>

namespace urchin {

    Container::Container(Position position, Size size, std::string skinName) :
            Widget(position, size),
            skinName(std::move(skinName)) {
        scrollbar = std::make_unique<Scrollbar>(*this, this->skinName);
    }

    std::shared_ptr<Container> Container::create(Widget* parent, Position position, Size size, std::string skinName) {
        return Widget::create<Container>(new Container(position, size, std::move(skinName)), parent);
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
        std::vector<std::shared_ptr<Widget>> copyChildren = getChildren();
        for(const auto& child : copyChildren) {
            if (!scrollbar->isScrollbarWidget(child.get())) {
                Widget::detachChild(child.get());
            }
        }
        scrollbar->onScrollableWidgetsUpdated();
    }

    int Container::getScrollShiftY() const {
        return scrollbar->getScrollShiftY();
    }

    void Container::onScrollbarMoved() const {
        Rectangle<int> containerRectangle = widgetRectangle();
        for(const auto& child : getChildren()) {
            if (!scrollbar->isScrollbarWidget(child.get())) {
                if (containerRectangle.collideWithRectangle(child->widgetRectangle())) {
                    child->setIsVisible(true); //TODO lazy loading
                } else {
                    child->setIsVisible(false); //TODO lazy unloading
                }
            }
        }
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
