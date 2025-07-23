#pragma once

#include "scene/ui/animation/AbstractUIWidgetAnimation.h"

namespace urchin {

    class UIAnimationTranslate final : public AbstractUIWidgetAnimation {
        public:
            UIAnimationTranslate(Widget&, const Point2<float>&, int = 1);

        protected:
            void initializeAnimation() override;
            void doAnimation(float) override;

        private:
            Point2<float> startPosition;
            Point2<float> endPosition;
            int maxRepeat;
            int executionCount;

            bool toEndPosition;
            float linearProgression;
    };

}
