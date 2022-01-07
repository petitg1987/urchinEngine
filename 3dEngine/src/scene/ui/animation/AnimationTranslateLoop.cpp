#include <scene/ui/animation/AnimationTranslateLoop.h>

namespace urchin {

    AnimationTranslateLoop::AnimationTranslateLoop(Widget& widget, const Point2<float>& start, const Point2<float>& end, float animationSpeed) :
            UIAnimation(widget),
            start(start),
            end(end),
            animationSpeed(animationSpeed),
            startToEnd(true),
            progression(0.0f) {

    }

    void AnimationTranslateLoop::animate(float dt) {
        if (startToEnd) {
            progression += dt * animationSpeed;
            if (progression > 1.0f) {
                startToEnd = false;
            }
        } else {
            progression -= dt * animationSpeed;
            if (progression < 0.0f) {
                startToEnd = true;
            }
        }

        Point2<float> newPosition = start.translate(start.vector(end) * progression);
        updatePosition(newPosition);
    }

    bool AnimationTranslateLoop::isCompleted() const {
        return false;
    }

}
