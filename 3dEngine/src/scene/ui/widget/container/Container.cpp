#include <scene/ui/widget/container/Container.h>
#include <scene/ui/widget/container/LazyWidget.h>

namespace urchin {

    Container::Container(Position position, Size size, std::string skinName) :
            Widget(position, size) {
        if (!skinName.empty()) {
            scrollbar = std::make_unique<Scrollbar>(*this, std::move(skinName));
        }
    }

    std::shared_ptr<Container> Container::create(Widget* parent, Position position, Size size) {
        return Widget::create<Container>(new Container(position, size, ""), parent);
    }

    std::shared_ptr<Container> Container::createScrollable(Widget* parent, Position position, Size size, std::string skinName) {
        return Widget::create<Container>(new Container(position, size, std::move(skinName)), parent);
    }

    void Container::onResize() {
        Widget::onResize();
        notifyChildrenUpdated();
    }

    void Container::notifyChildrenUpdated() const {
        if (scrollbar) {
            scrollbar->onScrollableWidgetsUpdated();
        }
    }

    WidgetType Container::getWidgetType() const {
        return WidgetType::CONTAINER;
    }

    void Container::addChild(const std::shared_ptr<Widget>& child) {
        Widget::addChild(child);
        notifyChildrenUpdated();
    }

    void Container::detachChild(Widget* child) {
        Widget::detachChild(child);
        notifyChildrenUpdated();
    }

    void Container::detachChildren() {
        Widget::detachChildren();
        notifyChildrenUpdated();
    }

    void Container::resetChildren() {
        std::vector<std::shared_ptr<Widget>> copyChildren = getChildren();
        for (const auto& child : copyChildren) {
            if (!scrollbar || !scrollbar->isScrollbarWidget(child.get())) {
                Widget::detachChild(child.get());
            }
        }
        notifyChildrenUpdated();
    }

    bool Container::isScrollable() const {
        return scrollbar != nullptr;
    }

    void Container::updateScrollShiftY(int shiftPixelPositionY) const {
        if (scrollbar) {
            scrollbar->updateScrollShiftY(shiftPixelPositionY);
        }
    }

    int Container::getScrollShiftY() const {
        if (scrollbar) {
            return scrollbar->getScrollShiftY();
        }
        return 0;
    }

    void Container::onScrollableContentUpdated() const {
        Rectangle2D<int> containerRectangle = widgetRectangle();
        for (const auto& child : getChildren()) {
            if (child->getWidgetType() == WidgetType::LAZY_WIDGET) {
                auto* lazyWidget = static_cast<LazyWidget*>(child.get());
                if (containerRectangle.collideWithRectangle(child->widgetRectangle())) {
                    lazyWidget->loadChildren();
                } else {
                    lazyWidget->unloadChildren();
                }
            }
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

    bool Container::onScrollEvent(double offsetY) {
        if (scrollbar) {
            return scrollbar->onScrollEvent(offsetY);
        }
        return true;
    }

}
