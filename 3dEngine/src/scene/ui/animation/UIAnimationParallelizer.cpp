#include <scene/ui/animation/UIAnimationParallelizer.h>

namespace urchin {

    UIAnimationParallelizer::UIAnimationParallelizer(std::vector<std::unique_ptr<UIAnimation>> animations) :
            animations(std::move(animations)) {

    }

    void UIAnimationParallelizer::animate(float dt) {
        for (std::unique_ptr<UIAnimation>& animation : animations) {
            animation->animate(dt);
        }
    }

    bool UIAnimationParallelizer::isCompleted() const {
        return std::ranges::all_of(animations, [](const auto& animation){ return animation->isCompleted(); });
    }

}