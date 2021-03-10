#include "scene/UI/widget/container/Container.h"

namespace urchin {

    Container::Container(Widget* parent, Position position) :
            Widget(parent, position, Size(0, 0, LengthType::PIXEL)) {

    }

    Container::Container(Position position) :
            Container(nullptr, position) {

    }

    void Container::createOrUpdateWidget() {

    }

    void Container::displayWidget(const ShaderVar&, float) {

    }

}
