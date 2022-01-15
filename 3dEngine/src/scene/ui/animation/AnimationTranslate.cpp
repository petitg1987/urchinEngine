#include <scene/ui/animation/AnimationTranslate.h>

namespace urchin {

    AnimationTranslate::AnimationTranslate(Widget& widget, const Point2<float>& endPosition, float animationSpeed) :
            UIAnimation(widget),
            endPosition(endPosition),
            animationSpeed(animationSpeed),
            progression(0.0f) {

    }

    void AnimationTranslate::initializeAnimation() {
        startPosition = Point2<float>(getWidget().getPosition().getX(), getWidget().getPosition().getY());
    }

    void AnimationTranslate::doAnimation(float dt) {
        if (!isCompleted()) {
            progression += dt * animationSpeed;

            Point2<float> newPosition = startPosition.translate(startPosition.vector(endPosition) * progression);
            updatePosition(newPosition);
        }
    }

    bool AnimationTranslate::isCompleted() const {
        return progression > 1.0f;
    }

}
