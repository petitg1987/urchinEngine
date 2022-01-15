#include <scene/ui/animation/UIAnimation.h>

namespace urchin {

    UIAnimation::UIAnimation(Widget& widget) :
            widget(widget),
            animationInitialized(false) {

    }

    void UIAnimation::animate(float dt) {
        if (!animationInitialized) {
            initializeAnimation();
            animationInitialized = true;
        }
        doAnimation(dt);
    }

    const Widget& UIAnimation::getWidget() const {
        return widget;
    }

    void UIAnimation::updatePosition(const Point2<float>& newPosition) {
        const Position& oldPosition = widget.getPosition();
        widget.updatePosition(Position(newPosition.X, oldPosition.getXType(), newPosition.Y, oldPosition.getYType(), oldPosition.getRelativeTo(), oldPosition.getReferencePoint()));
    }

}