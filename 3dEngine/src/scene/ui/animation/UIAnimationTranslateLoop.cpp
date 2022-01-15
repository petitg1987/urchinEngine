#include <scene/ui/animation/UIAnimationTranslateLoop.h>

namespace urchin {

    UIAnimationTranslateLoop::UIAnimationTranslateLoop(Widget& widget, const Point2<float>& endPosition, float animationSpeed) :
            AbstractUIWidgetAnimation(widget),
            endPosition(endPosition),
            animationSpeed(animationSpeed),
            startToEnd(true),
            progression(0.0f) {

    }

    void UIAnimationTranslateLoop::initializeAnimation() {
        startPosition = Point2<float>(getWidget().getPosition().getX(), getWidget().getPosition().getY());
    }

    void UIAnimationTranslateLoop::doAnimation(float dt) {
        if (startToEnd) {
            progression += dt * animationSpeed;
            if (progression > 1.0f) {
                startToEnd = false;
            }
        } else {
            progression -= dt * animationSpeed;
            if (progression < 0.0f) {
                startToEnd = true;
            }
        }
        updatePosition(startPosition.translate(startPosition.vector(endPosition) * progression));
    }

    bool UIAnimationTranslateLoop::isCompleted() const {
        return false;
    }

}
