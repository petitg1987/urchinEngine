#include <scene/ui/animation/UIAnimationSequencer.h>

namespace urchin {

    UIAnimationSequencer::UIAnimationSequencer(std::vector<std::unique_ptr<AbstractUIAnimation>> animations) :
            currentAnimationIndex(0),
            animations(std::move(animations)) {

    }

    void UIAnimationSequencer::doAnimation(float dt) {
        if (currentAnimationIndex < animations.size()) {
            const std::unique_ptr<AbstractUIAnimation>& currentAnimation = animations[currentAnimationIndex];

            currentAnimation->animate(dt);
            if (currentAnimation->isCompleted()) {
                currentAnimationIndex++;
                if (currentAnimationIndex >= animations.size()) {
                    markCompleted();
                }
            }
        }
    }

    unsigned int UIAnimationSequencer::getAnimationIndex() const {
        return currentAnimationIndex;
    }

}