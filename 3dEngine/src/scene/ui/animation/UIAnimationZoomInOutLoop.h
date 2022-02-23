#pragma once

#include <scene/ui/animation/AbstractUIWidgetAnimation.h>

namespace urchin {

    class UIAnimationZoomInOutLoop : public AbstractUIWidgetAnimation {
        public:
            UIAnimationZoomInOutLoop(Widget&, const Vector2<float>&);

            bool isCompleted() const override;

        protected:
            void initializeAnimation() override;
            void doAnimation(float) override;

        private:
            Vector2<float> startScale;
            Vector2<float> endScale;

            bool zoomToEndSize;
            float linearProgression;
    };

}
