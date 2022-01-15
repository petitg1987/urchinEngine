#pragma once

#include <scene/ui/animation/AbstractUIWidgetAnimation.h>

namespace urchin {

    class UIAnimationTranslateLoop : public AbstractUIWidgetAnimation {
        public:
            UIAnimationTranslateLoop(Widget&, const Point2<float>&, float);

            void initializeAnimation() override;
            void doAnimation(float) override;

            bool isCompleted() const override;

        private:
            Point2<float> startPosition;
            Point2<float> endPosition;
            float animationSpeed;

            bool startToEnd;
            float progression;
    };

}
