#pragma once

#include <scene/ui/animation/UIAnimation.h>

namespace urchin {

    class AnimationTranslate : public UIAnimation {
        public:
            AnimationTranslate(Widget&, const Point2<float>&, const Point2<float>&, float);

            void animate(float) override;
            bool isCompleted() const override;

        private:
            Point2<float> start;
            Point2<float> end;
            float animationSpeed;

            float progression;
    };

}
