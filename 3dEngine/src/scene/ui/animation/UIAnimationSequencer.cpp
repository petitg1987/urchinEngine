#include <scene/ui/animation/UIAnimationSequencer.h>

namespace urchin {

    UIAnimationSequencer::UIAnimationSequencer(std::vector<std::unique_ptr<UIAnimation>> animations) :
            currentAnimationIndex(0),
            animations(std::move(animations)) {

    }

    void UIAnimationSequencer::animate(float dt) {
        if (!isCompleted()) {
            std::unique_ptr<UIAnimation>& currentAnimation = animations[currentAnimationIndex];

            currentAnimation->animate(dt);
            if (currentAnimation->isCompleted()) {
                currentAnimationIndex++;
            }
        }
    }

    bool UIAnimationSequencer::isCompleted() const {
        return currentAnimationIndex >= animations.size();
    }

}