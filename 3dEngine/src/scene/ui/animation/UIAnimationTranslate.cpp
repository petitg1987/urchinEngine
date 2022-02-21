#include <scene/ui/animation/UIAnimationTranslate.h>

namespace urchin {

    UIAnimationTranslate::UIAnimationTranslate(Widget& widget, const Point2<float>& endPosition) :
            AbstractUIWidgetAnimation(widget),
            endPosition(endPosition),
            linearProgression(0.0f) {

    }

    void UIAnimationTranslate::initializeAnimation() {
        startPosition = Point2<float>(getWidget().getPosition().getX(), getWidget().getPosition().getY());
    }

    void UIAnimationTranslate::doAnimation(float dt) {
        linearProgression += dt * getAnimationSpeed();
        float animationProgression = computeProgression(linearProgression);

        updatePosition(startPosition.translate(startPosition.vector(endPosition) * animationProgression));
    }

    bool UIAnimationTranslate::isCompleted() const {
        return linearProgression > 1.0f;
    }

}
