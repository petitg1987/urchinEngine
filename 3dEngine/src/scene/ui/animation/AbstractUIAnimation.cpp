#include <scene/ui/animation/AbstractUIAnimation.h>

namespace urchin {

    AbstractUIAnimation::AbstractUIAnimation() :
            animationInitialized(false) {

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