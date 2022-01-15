#include <scene/ui/animation/AnimationTranslateLoop.h>

namespace urchin {

    AnimationTranslateLoop::AnimationTranslateLoop(Widget& widget, const Point2<float>& endPosition, float animationSpeed) :
            UIAnimation(widget),
            endPosition(endPosition),
            animationSpeed(animationSpeed),
            startToEnd(true),
            progression(0.0f) {

    }

    void AnimationTranslateLoop::initializeAnimation() {
        startPosition = Point2<float>(getWidget().getPosition().getX(), getWidget().getPosition().getY());
    }

    void AnimationTranslateLoop::doAnimation(float dt) {
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

        Point2<float> newPosition = startPosition.translate(startPosition.vector(endPosition) * progression);
        updatePosition(newPosition);
    }

    bool AnimationTranslateLoop::isCompleted() const {
        return false;
    }

}
