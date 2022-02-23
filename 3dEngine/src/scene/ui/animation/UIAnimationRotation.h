#pragma once

#include <scene/ui/animation/AbstractUIWidgetAnimation.h>

namespace urchin {

    class UIAnimationRotation : public AbstractUIWidgetAnimation {
        public:
            UIAnimationRotation(Widget&, float, int = 1);

            bool isCompleted() const override;

        protected:
            void initializeAnimation() override;
            void doAnimation(float) override;

        private:
            float startRotation;
            float endRotation;
            int maxRepeat;
            int executionCount;

            bool toEndRotation;
            float linearProgression;
    };

}
