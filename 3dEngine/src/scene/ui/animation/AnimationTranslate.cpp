#include <scene/ui/animation/AnimationTranslate.h>

namespace urchin {

    AnimationTranslate::AnimationTranslate(const Point2<float>& start, const Point2<float>& end, float animationSpeed) :
            start(start),
            end(end),
            animationSpeed(animationSpeed),
            startToEnd(true),
            progression(0.0f) {

    }

    Point2<float> AnimationTranslate::getPosition() const {
        return start.translate(start.vector(end) * progression);
    }

    void AnimationTranslate::animate(float dt) {
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
    }

}
