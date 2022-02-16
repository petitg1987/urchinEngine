#pragma once

#include <scene/ui/animation/AbstractUIWidgetAnimation.h>

namespace urchin {

    class UIAnimationTranslate : public AbstractUIWidgetAnimation {
        public:
            UIAnimationTranslate(Widget&, const Point2<float>&, float);

            void initializeAnimation() override;
            void doAnimation(float) override;

            bool isCompleted() const override;

        private:
            Point2<float> startPosition;
            Point2<float> endPosition;
            float animationSpeed;

            float progression;
    };

}