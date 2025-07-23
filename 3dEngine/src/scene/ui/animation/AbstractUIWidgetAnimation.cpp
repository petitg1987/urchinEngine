#include "scene/ui/animation/AbstractUIWidgetAnimation.h"

namespace urchin {

    AbstractUIWidgetAnimation::AbstractUIWidgetAnimation(Widget& widget) :
            widget(widget) {

    }

    const Widget& AbstractUIWidgetAnimation::getWidget() const {
        return widget;
    }

    void AbstractUIWidgetAnimation::updatePosition(const Point2<float>& newPosition) const {
        const Position& oldPosition = widget.getPosition();
        widget.updatePosition(Position(newPosition.X, oldPosition.getXType(), newPosition.Y, oldPosition.getYType(), oldPosition.getRelativeTo(), oldPosition.getReferencePoint()));
    }

    void AbstractUIWidgetAnimation::updateScale(const Vector2<float>& newScale) const {
        widget.updateScale(newScale);
    }

    void AbstractUIWidgetAnimation::updateRotation(float rotation) const {
        widget.updateRotation(rotation);
    }

    void AbstractUIWidgetAnimation::updateAlphaFactor(float alphaFactor) const {
        widget.updateAlphaFactor(alphaFactor);
    }

}