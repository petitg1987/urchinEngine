#include <scene/ui/widget/container/Container.h>

namespace urchin {

    Container::Container(Widget* parent, Position position) :
            Widget(parent, position, Size(0, 0, LengthType::PIXEL)) {

    }

    void Container::createOrUpdateWidget() {

    }

    void Container::prepareWidgetRendering(float) {

    }

}
