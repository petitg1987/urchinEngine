#include <scene/ui/animation/UIAnimationTranslate.h>

namespace urchin {

    UIAnimationTranslate::UIAnimationTranslate(Widget& widget, const Point2<float>& endPosition, int maxRepeat) :
            AbstractUIWidgetAnimation(widget),
            endPosition(endPosition),
            maxRepeat(maxRepeat),
            executionCount(0),
            startToEnd(true),
            linearProgression(0.0f) {

    }

    void UIAnimationTranslate::initializeAnimation() {
        startPosition = Point2<float>(getWidget().getPosition().getX(), getWidget().getPosition().getY());
    }

    void UIAnimationTranslate::doAnimation(float dt) {
        if (startToEnd) {
            linearProgression += dt * getAnimationSpeed();
            if (linearProgression > 1.0f) {
                startToEnd = false;
                executionCount++;
            }
        } else {
            linearProgression -= dt * getAnimationSpeed();
            if (linearProgression < 0.0f) {
                startToEnd = true;
                executionCount++;
            }
        }

        float animationProgress = computeProgression(linearProgression) ;
        updatePosition(startPosition.translate(startPosition.vector(endPosition) * animationProgress));
    }

    bool UIAnimationTranslate::isCompleted() const {
        if (maxRepeat < 0) {
            return false;
        }
        return executionCount >= maxRepeat;
    }

}
