#pragma once

#include <scene/ui/animation/AbstractUIAnimation.h>

namespace urchin {

    class UIAnimationDelay : public AbstractUIAnimation {
        public:
            UIAnimationDelay(float);

            void doAnimation(float) override;

            bool isCompleted() const override;

        private:
            float delaySecond;
            float elapsedTimeSecond;
    };

}

