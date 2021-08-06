#include <scene/ui/widget/container/LazyWidget.h>

namespace urchin {

    LazyWidget::LazyWidget(Position position, Size size) :
            Widget(position, size),
            isLoaded(false) {

    }

    std::shared_ptr<LazyWidget> LazyWidget::create(Widget* parent, Position position, Size size) {
        return Widget::create<LazyWidget>(new LazyWidget(position, size), parent);
    }

    void LazyWidget::setupLazyChildren(std::function<void()> loadChildrenFunction) {
        this->loadChildrenFunction = std::move(loadChildrenFunction);
    }

    void LazyWidget::loadChildren() {
        if (!isLoaded && loadChildrenFunction) {
            loadChildrenFunction();
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