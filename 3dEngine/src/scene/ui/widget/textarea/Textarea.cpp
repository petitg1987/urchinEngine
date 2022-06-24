#include <scene/ui/widget/textarea/Textarea.h>

namespace urchin {

    Textarea::Textarea(Position position, std::string skinName) :
            Widget(position, Size(0, 0, LengthType::PIXEL)),
            skinName(std::move(skinName)) {

    }

    std::shared_ptr<Textarea> Textarea::create(Widget* parent, Position position, std::string skinName) {
        return Widget::create<Textarea>(new Textarea(position, std::move(skinName)), parent);
    }

    WidgetType Textarea::getWidgetType() const {
        return WidgetType::TEXTAREA;
    }

    void Textarea::createOrUpdateWidget() {

    }

}
