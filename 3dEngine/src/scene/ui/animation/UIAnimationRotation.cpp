#include "scene/ui/animation/UIAnimationRotation.h"

namespace urchin {

    UIAnimationRotation::UIAnimationRotation(Widget& widget, float endRotation, int maxRepeat) :
            AbstractUIWidgetAnimation(widget),
            startRotation(0.0f),
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
                linearProgression = 1.0f;
                toEndRotation = false;
                executionCount++;
            }
        } else {
            linearProgression -= dt * getAnimationSpeed();
            if (linearProgression < 0.0f) {
                linearProgression = 0.0f;
                toEndRotation = true;
                executionCount++;
            }
        }

        if (maxRepeat >= 0 && executionCount >= maxRepeat) {
            updateRotation((maxRepeat % 2 == 0) ? startRotation : endRotation);
            markCompleted();
        } else {
            float animationProgress = computeProgression(linearProgression);
            updateRotation(startRotation + (endRotation - startRotation) * animationProgress);
        }
    }

}
