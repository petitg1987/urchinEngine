#include <scene/ui/animation/UIAnimationFade.h>

namespace urchin {

    UIAnimationFade::UIAnimationFade(Widget& widget, float endFadeValue) :
            AbstractUIWidgetAnimation(widget),
            startFadeValue(1.0f),
            endFadeValue(endFadeValue),
            linearProgression(0.0f) {

    }

    void UIAnimationFade::initializeAnimation() {
        startFadeValue = getWidget().getAlphaFactor();
    }

    void UIAnimationFade::doAnimation(float dt) {
        linearProgression += dt * getAnimationSpeed();
        if (linearProgression > 1.0f) {
            updateAlphaFactor(endFadeValue);
            markCompleted();
        } else {
            float animationProgress = computeProgression(linearProgression);
            updateAlphaFactor(startFadeValue + (endFadeValue - startFadeValue) * animationProgress);
        }
    }

}
