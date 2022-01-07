#include <scene/ui/animation/UIAnimation.h>

namespace urchin {

    UIAnimation::UIAnimation(Widget& widget) :
            widget(widget) {

    }

    void UIAnimation::updatePosition(const Point2<float>& newPosition) {
        const Position& oldPosition = widget.getPosition();
        widget.updatePosition(Position(newPosition.X, oldPosition.getXType(), newPosition.Y, oldPosition.getYType(), oldPosition.getRelativeTo(), oldPosition.getReferencePoint()));
    }

}