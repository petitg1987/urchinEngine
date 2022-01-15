#pragma once

#include <scene/ui/animation/UIAnimation.h>

namespace urchin {

    class AnimationTranslateLoop : public UIAnimation {
        public:
            AnimationTranslateLoop(Widget&, const Point2<float>&, float);

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
