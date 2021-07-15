#include <scene/ui/widget/container/Container.h>

namespace urchin {

    Container::Container(Position position) :
            Widget(position, Size(0, 0, LengthType::PIXEL)) {

    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position) {
        return create<Container>(new Container(position), parent);
    }

    void Container::createOrUpdateWidget() {

    }

    void Container::prepareWidgetRendering(float) {

    }

}
