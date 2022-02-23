#include <scene/ui/animation/UIAnimationZoomInOutLoop.h>

namespace urchin {

    UIAnimationZoomInOutLoop::UIAnimationZoomInOutLoop(Widget& widget, const Vector2<float>& endScale) :
            AbstractUIWidgetAnimation(widget),
            endScale(endScale),
            zoomToEndSize(true),
            linearProgression(0.0f) {

    }

    void UIAnimationZoomInOutLoop::initializeAnimation() {
        startScale = getWidget().getScale();
    }

    void UIAnimationZoomInOutLoop::doAnimation(float dt) {
        if (zoomToEndSize) {
            linearProgression += dt * getAnimationSpeed();
            if (linearProgression > 1.0f) {
                zoomToEndSize = false;
            }
        } else {
            linearProgression -= dt * getAnimationSpeed();
            if (linearProgression < 0.0f) {
                zoomToEndSize = true;
            }
        }

        float animationProgress = computeProgression(linearProgression) ;
        updateScale(startScale + (endScale - startScale) * animationProgress);
    }

    bool UIAnimationZoomInOutLoop::isCompleted() const {
        return false;
    }

}
