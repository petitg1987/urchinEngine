#include <scene/ui/widget/container/Container.h>

namespace urchin {

    Container::Container(Position position) :
            Widget(position, Size(0, 0, LengthType::PIXEL)) {

    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position) {
        auto widget = std::shared_ptr<Container>(new Container(position));
        if(parent) {
            parent->addChild(widget);
        }
        return widget;
    }

    void Container::createOrUpdateWidget() {

    }

    void Container::prepareWidgetRendering(float) {

    }

}
