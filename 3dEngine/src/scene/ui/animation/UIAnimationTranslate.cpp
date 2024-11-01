#include <scene/ui/animation/UIAnimationTranslate.h>

namespace urchin {

    UIAnimationTranslate::UIAnimationTranslate(Widget& widget, const Point2<float>& endPosition, int maxRepeat) :
            AbstractUIWidgetAnimation(widget),
            endPosition(endPosition),
            maxRepeat(maxRepeat),
            executionCount(0),
            toEndPosition(true),
            linearProgression(0.0f) {

    }

    void UIAnimationTranslate::initializeAnimation() {
        startPosition = Point2(getWidget().getPosition().getX(), getWidget().getPosition().getY());
    }

    void UIAnimationTranslate::doAnimation(float dt) {
        if (toEndPosition) {
            linearProgression += dt * getAnimationSpeed();
            if (linearProgression > 1.0f) {
                linearProgression = 1.0f;
                toEndPosition = false;
                executionCount++;
            }
        } else {
            linearProgression -= dt * getAnimationSpeed();
            if (linearProgression < 0.0f) {
                linearProgression = 0.0f;
                toEndPosition = true;
                executionCount++;
            }
        }

        if (maxRepeat >= 0 && executionCount >= maxRepeat) {
            updatePosition((maxRepeat % 2 == 0) ? startPosition : endPosition);
            markCompleted();
        } else {
            float animationProgress = computeProgression(linearProgression) ;
            updatePosition(startPosition.translate(startPosition.vector(endPosition) * animationProgress));
        }
    }

}
