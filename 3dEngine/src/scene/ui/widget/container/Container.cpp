#include <scene/ui/widget/container/Container.h>

namespace urchin {

    Container::Container(Position position, Size size) :
            Widget(position, size),
            scissorEnabled(true) {

    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position) {
        return create<Container>(new Container(position, Size(100.0f, 100.0f, LengthType::SCREEN_PERCENT)), parent);
    }

    std::shared_ptr<Container> Container::newContainer(Widget* parent, Position position, Size size) {
        return create<Container>(new Container(position, size), parent);
    }

    void Container::enableScissor(bool scissorEnabled) {
        this->scissorEnabled = scissorEnabled;
    }

    bool Container::isScissorEnabled() const {
        return scissorEnabled;
    }

    void Container::createOrUpdateWidget() {

    }

    void Container::prepareWidgetRendering(float) {

    }

}
