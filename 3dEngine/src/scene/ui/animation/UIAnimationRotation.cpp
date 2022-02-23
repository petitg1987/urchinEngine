#include <scene/ui/animation/UIAnimationRotation.h>

namespace urchin {

    UIAnimationRotation::UIAnimationRotation(Widget& widget, float endRotation, int maxRepeat) :
            AbstractUIWidgetAnimation(widget),
            endRotation(endRotation),
            maxRepeat(maxRepeat),
            executionCount(0),
            toEndRotation(true),
            linearProgression(0.0f) {

    }

    void UIAnimationRotation::initializeAnimation() {
        startRotation = getWidget().getRotation();
    }

    void UIAnimationRotation::doAnimation(float dt) {
        if (toEndRotation) {
            linearProgression += dt * getAnimationSpeed();
            if (linearProgression > 1.0f) {
                toEndRotation = false;
                executionCount++;
            }
        } else {
            linearProgression -= dt * getAnimationSpeed();
            if (linearProgression < 0.0f) {
                toEndRotation = true;
                executionCount++;
            }
        }

        float animationProgress = computeProgression(linearProgression) ;
        updateRotation(startRotation + (endRotation - startRotation) * animationProgress);
    }

    bool UIAnimationRotation::isCompleted() const {
        if (maxRepeat < 0) {
            return false;
        }
        return executionCount >= maxRepeat;
    }

}
