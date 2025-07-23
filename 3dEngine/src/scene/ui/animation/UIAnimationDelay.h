#pragma once

#include "scene/ui/animation/AbstractUIAnimation.h"

namespace urchin {

    class UIAnimationDelay final : public AbstractUIAnimation {
        public:
            explicit UIAnimationDelay(float);

        protected:
            void doAnimation(float) override;

        private:
            float delaySecond;
            float elapsedTimeSecond;
    };

}

