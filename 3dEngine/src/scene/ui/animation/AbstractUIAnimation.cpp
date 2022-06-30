#include <scene/ui/animation/AbstractUIAnimation.h>

namespace urchin {

    AbstractUIAnimation::AbstractUIAnimation() :
            animationInitialized(false),
            animationCompleted(false),
            animationSpeed(1.0f) {

    }

    void AbstractUIAnimation::setupCompletedCallback(std::function<void()> completedCallback) {
        this->completedCallback = std::move(completedCallback);
    }

    void AbstractUIAnimation::setupAnimationSpeed(float animationSpeed, std::unique_ptr<Bezier> bezier) {
        this->animationSpeed = animationSpeed;
        this->bezier = std::move(bezier);
    }

    float AbstractUIAnimation::getAnimationSpeed() const {
        return animationSpeed;
    }

    float AbstractUIAnimation::computeProgression(float linearProgression) const {
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

    bool AbstractUIAnimation::isCompleted() const {
        return animationCompleted;
    }

    void AbstractUIAnimation::markCompleted() {
        animationCompleted = true;
        if (completedCallback) {
            completedCallback();
        }
    }

    void AbstractUIAnimation::initializeAnimation() {
        //can be overridden
    }

}