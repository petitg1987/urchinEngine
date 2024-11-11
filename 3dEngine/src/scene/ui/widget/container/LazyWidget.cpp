#include <scene/ui/widget/container/LazyWidget.h>

namespace urchin {

    LazyWidget::LazyWidget(Position position, Size size, std::function<void(LazyWidget*)> loadChildrenFunction) :
            Widget(std::move(position), size),
            loadChildrenFunction(std::move(loadChildrenFunction)),
            isLoaded(false) {

    }

    std::shared_ptr<LazyWidget> LazyWidget::create(Widget* parent, Position position, Size size, std::function<void(LazyWidget*)> loadChildrenFunction) {
        return Widget::create<LazyWidget>(new LazyWidget(std::move(position), size, std::move(loadChildrenFunction)), parent);
    }

    WidgetType LazyWidget::getWidgetType() const {
        return WidgetType::LAZY_WIDGET;
    }

    void LazyWidget::loadChildren() {
        if (!isLoaded) {
            loadChildrenFunction(this);
            isLoaded = true;
        }
    }

    void LazyWidget::unloadChildren() {
        if (isLoaded) {
            Widget::detachChildren();
            isLoaded = false;
        }
    }

    void LazyWidget::createOrUpdateWidget() {
        //nothing to do
    }

}