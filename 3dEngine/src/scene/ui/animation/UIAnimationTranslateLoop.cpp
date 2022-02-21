#include <scene/ui/animation/UIAnimationTranslateLoop.h>

namespace urchin {

    UIAnimationTranslateLoop::UIAnimationTranslateLoop(Widget& widget, const Point2<float>& endPosition) :
            AbstractUIWidgetAnimation(widget),
            endPosition(endPosition),
            startToEnd(true),
            linearProgression(0.0f) {

    }

    void UIAnimationTranslateLoop::initializeAnimation() {
        startPosition = Point2<float>(getWidget().getPosition().getX(), getWidget().getPosition().getY());
    }

    void UIAnimationTranslateLoop::doAnimation(float dt) {
        if (startToEnd) {
            linearProgression += dt * getAnimationSpeed();
            if (linearProgression > 1.0f) {
                startToEnd = false;
            }
        } else {
            linearProgression -= dt * getAnimationSpeed();
            if (linearProgression < 0.0f) {
                startToEnd = true;
            }
        }

        float animationProgress = computeProgression(linearProgression) ;
        updatePosition(startPosition.translate(startPosition.vector(endPosition) * animationProgress));
    }

    bool UIAnimationTranslateLoop::isCompleted() const {
        return false;
    }

}
