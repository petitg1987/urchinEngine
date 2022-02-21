#include <scene/ui/animation/AbstractUIAnimation.h>

namespace urchin {

    AbstractUIAnimation::AbstractUIAnimation() :
            animationInitialized(false),
            animationSpeed(1.0f) {

    }

    void AbstractUIAnimation::setupAnimationSpeed(float animationSpeed, std::unique_ptr<Bezier> bezier) {
        this->animationSpeed = animationSpeed;
        this->bezier = std::move(bezier);
    }

    float AbstractUIAnimation::getAnimationSpeed() const {
        return animationSpeed;
    }

    float AbstractUIAnimation::computeProgression(float linearProgression) {
        if (bezier) {
            return bezier->computeProgression(linearProgression);
        }
        return linearProgression;
    }

    void AbstractUIAnimation::animate(float dt) {
        if (!isCompleted()) {
            if (!animationInitialized) {
                initializeAnimation();
                animationInitialized = true;
            }
            doAnimation(dt);
        }
    }

    void AbstractUIAnimation::initializeAnimation() {
        //can be overridden
    }

}