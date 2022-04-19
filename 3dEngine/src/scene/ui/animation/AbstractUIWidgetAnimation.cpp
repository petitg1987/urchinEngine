#include <scene/ui/animation/AbstractUIWidgetAnimation.h>

namespace urchin {

    AbstractUIWidgetAnimation::AbstractUIWidgetAnimation(Widget& widget) :
            widget(widget) {

    }

    const Widget& AbstractUIWidgetAnimation::getWidget() const {
        return widget;
    }

    void AbstractUIWidgetAnimation::updatePosition(const Point2<float>& newPosition) {
        const Position& oldPosition = widget.getPosition();
        widget.updatePosition(Position(newPosition.X, oldPosition.getXType(), newPosition.Y, oldPosition.getYType(), oldPosition.getRelativeTo(), oldPosition.getReferencePoint()));
    }

    void AbstractUIWidgetAnimation::updateScale(const Vector2<float>& newScale) {
        widget.updateScale(newScale);
    }

    void AbstractUIWidgetAnimation::updateRotation(float rotation) {
        widget.updateRotation(rotation);
    }

    void AbstractUIWidgetAnimation::updateAlphaFactor(float alphaFactor) {
        widget.updateAlphaFactor(alphaFactor);
    }

}