#pragma once

#include <scene/ui/animation/UIAnimation.h>

namespace urchin {

    class UIAnimationSequencer {
        public:
            explicit UIAnimationSequencer(std::vector<std::unique_ptr<UIAnimation>>);

            void animate(float);

        private:
            unsigned int currentAnimationIndex;
            std::vector<std::unique_ptr<UIAnimation>> animations;
    };

}