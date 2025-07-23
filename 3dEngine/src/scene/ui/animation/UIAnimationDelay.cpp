#include "scene/ui/animation/UIAnimationDelay.h"

namespace urchin {

    UIAnimationDelay::UIAnimationDelay(float delaySecond) :
            delaySecond(delaySecond),
            elapsedTimeSecond(0.0f) {

    }

    void UIAnimationDelay::doAnimation(float dt) {
        elapsedTimeSecond += dt;

        if (elapsedTimeSecond > delaySecond) {
            markCompleted();
        }
    }

}