#pragma once

#include <scene/ui/animation/AbstractUIWidgetAnimation.h>

namespace urchin {

    class UIAnimationTranslateLoop : public AbstractUIWidgetAnimation {
        public:
            UIAnimationTranslateLoop(Widget&, const Point2<float>&);

            bool isCompleted() const override;

        protected:
            void initializeAnimation() override;
            void doAnimation(float) override;

        private:
            Point2<float> startPosition;
            Point2<float> endPosition;

            bool startToEnd;
            float linearProgression;
    };

}
