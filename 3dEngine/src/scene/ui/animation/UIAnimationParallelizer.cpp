#include "scene/ui/animation/UIAnimationParallelizer.h"

namespace urchin {

    UIAnimationParallelizer::UIAnimationParallelizer(std::vector<std::unique_ptr<AbstractUIAnimation>> animations) :
            animations(std::move(animations)) {

    }

    void UIAnimationParallelizer::doAnimation(float dt) {
        for (const std::unique_ptr<AbstractUIAnimation>& animation : animations) {
            animation->animate(dt);
        }

        if (std::ranges::all_of(animations, [](const auto& animation){ return animation->isCompleted(); })) {
            markCompleted();
        }
    }

}
