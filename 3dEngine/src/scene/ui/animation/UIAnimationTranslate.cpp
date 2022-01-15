#include <scene/ui/animation/UIAnimationTranslate.h>

namespace urchin {

    UIAnimationTranslate::UIAnimationTranslate(Widget& widget, const Point2<float>& endPosition, float animationSpeed) :
            AbstractUIWidgetAnimation(widget),
            endPosition(endPosition),
            animationSpeed(animationSpeed),
            progression(0.0f) {

    }

    void UIAnimationTranslate::initializeAnimation() {
        startPosition = Point2<float>(getWidget().getPosition().getX(), getWidget().getPosition().getY());
    }

    void UIAnimationTranslate::doAnimation(float dt) {
        progression += dt * animationSpeed;
        updatePosition(startPosition.translate(startPosition.vector(endPosition) * progression));
    }

    bool UIAnimationTranslate::isCompleted() const {
        return progression > 1.0f;
    }

}
