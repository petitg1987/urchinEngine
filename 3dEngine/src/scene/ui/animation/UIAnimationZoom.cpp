#include <scene/ui/animation/UIAnimationZoom.h>

namespace urchin {

    UIAnimationZoom::UIAnimationZoom(Widget& widget, const Vector2<float>& endScale, int maxRepeat) :
            AbstractUIWidgetAnimation(widget),
            endScale(endScale),
            maxRepeat(maxRepeat),
            executionCount(0),
            zoomToEndSize(true),
            linearProgression(0.0f) {

    }

    void UIAnimationZoom::initializeAnimation() {
        startScale = getWidget().getScale();
    }

    void UIAnimationZoom::doAnimation(float dt) {
        if (zoomToEndSize) {
            linearProgression += dt * getAnimationSpeed();
            if (linearProgression > 1.0f) {
                zoomToEndSize = false;
                executionCount++;
            }
        } else {
            linearProgression -= dt * getAnimationSpeed();
            if (linearProgression < 0.0f) {
                zoomToEndSize = true;
                executionCount++;
            }
        }

        float animationProgress = computeProgression(linearProgression) ;
        updateScale(startScale + (endScale - startScale) * animationProgress);
    }

    bool UIAnimationZoom::isCompleted() const {
        if (maxRepeat < 0) {
            return false;
        }
        return executionCount >= maxRepeat;
    }

}
