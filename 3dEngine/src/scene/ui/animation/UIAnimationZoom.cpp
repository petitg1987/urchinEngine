#include "scene/ui/animation/UIAnimationZoom.h"

namespace urchin {

    UIAnimationZoom::UIAnimationZoom(Widget& widget, const Vector2<float>& endScale, int maxRepeat) :
            AbstractUIWidgetAnimation(widget),
            endScale(endScale),
            maxRepeat(maxRepeat),
            executionCount(0),
            toEndScale(true),
            linearProgression(0.0f) {

    }

    void UIAnimationZoom::initializeAnimation() {
        startScale = getWidget().getScale();
    }

    void UIAnimationZoom::doAnimation(float dt) {
        if (toEndScale) {
            linearProgression += dt * getAnimationSpeed();
            if (linearProgression > 1.0f) {
                linearProgression = 1.0f;
                toEndScale = false;
                executionCount++;
            }
        } else {
            linearProgression -= dt * getAnimationSpeed();
            if (linearProgression < 0.0f) {
                linearProgression = 0.0f;
                toEndScale = true;
                executionCount++;
            }
        }

        if (maxRepeat >= 0 && executionCount >= maxRepeat) {
            updateScale((maxRepeat % 2 == 0) ? startScale : endScale);
            markCompleted();
        } else {
            float animationProgress = computeProgression(linearProgression) ;
            updateScale(startScale + (endScale - startScale) * animationProgress);
        }
    }

}
