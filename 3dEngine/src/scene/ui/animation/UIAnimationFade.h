#pragma once

#include <scene/ui/animation/AbstractUIWidgetAnimation.h>

namespace urchin {

    class UIAnimationFade : public AbstractUIWidgetAnimation {
        public:
            UIAnimationFade(Widget&, float);

        protected:
            void initializeAnimation() override;
            void doAnimation(float) override;

        private:
            float startFadeValue;
            float endFadeValue;

            float linearProgression;
    };

}
