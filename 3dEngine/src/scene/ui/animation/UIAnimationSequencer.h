#pragma once

#include <scene/ui/animation/AbstractUIAnimation.h>

namespace urchin {

    class UIAnimationSequencer : public AbstractUIAnimation {
        public:
            explicit UIAnimationSequencer(std::vector<std::unique_ptr<AbstractUIAnimation>>);

            void doAnimation(float) override;
            bool isCompleted() const override;

        private:
            unsigned int currentAnimationIndex;
            std::vector<std::unique_ptr<AbstractUIAnimation>> animations;
    };

}