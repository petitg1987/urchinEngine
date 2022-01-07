#include <scene/ui/animation/AnimationTranslate.h>

namespace urchin {

    AnimationTranslate::AnimationTranslate(Widget& widget, const Point2<float>& start, const Point2<float>& end, float animationSpeed) :
            UIAnimation(widget),
            start(start),
            end(end),
            animationSpeed(animationSpeed),
            progression(0.0f) {

    }

    void AnimationTranslate::animate(float dt) {
        if (!isCompleted()) {
            progression += dt * animationSpeed;

            Point2<float> newPosition = start.translate(start.vector(end) * progression);
            updatePosition(newPosition);
        }
    }

    bool AnimationTranslate::isCompleted() const {
        return progression > 1.0f;
    }

}
