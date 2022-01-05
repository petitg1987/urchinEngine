#pragma once

#include <scene/ui/animation/UIAnimation.h>

namespace urchin {

    class AnimationTranslate : public UIAnimation {
        public:
            AnimationTranslate(const Point2<float>&, const Point2<float>&, float);

            Point2<float> getPosition() const override;

            void animate(float) override;

        private:
            Point2<float> start;
            Point2<float> end;
            float animationSpeed;

            bool startToEnd;
            float progression;
    };

}
