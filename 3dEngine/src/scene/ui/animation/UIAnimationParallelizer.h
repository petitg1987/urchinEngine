#pragma once

#include <scene/ui/animation/UIAnimation.h>

namespace urchin {

    class UIAnimationParallelizer {
        public:
            explicit UIAnimationParallelizer(std::vector<std::unique_ptr<UIAnimation>>);

            void animate(float);
            bool isCompleted() const;

        private:
            std::vector<std::unique_ptr<UIAnimation>> animations;
    };

}