#include <scene/ui/widget/container/Container.h>

namespace urchin {

    Container::Container(Position position, Size size) :
            Widget(position, size) {

    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position) {
        return create<Container>(new Container(position, Size(100.0f, 100.0f, LengthType::PERCENTAGE)), parent);
    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position, Size size) {
        return create<Container>(new Container(position, size), parent);
    }

    void Container::createOrUpdateWidget() {

    }

    void Container::prepareWidgetRendering(float) {

    }

}
