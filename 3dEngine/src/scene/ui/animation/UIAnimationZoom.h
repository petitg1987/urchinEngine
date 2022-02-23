#pragma once

#include <scene/ui/animation/AbstractUIWidgetAnimation.h>

namespace urchin {

    class UIAnimationZoom : public AbstractUIWidgetAnimation {
        public:
            UIAnimationZoom(Widget&, const Vector2<float>&, int = 1);

            bool isCompleted() const override;

        protected:
            void initializeAnimation() override;
            void doAnimation(float) override;

        private:
            Vector2<float> startScale;
            Vector2<float> endScale;
            int maxRepeat;
            int executionCount;

            bool zoomToEndSize;
            float linearProgression;
    };

}
