#pragma once

#include "scene/ui/widget/Widget.h"
#include "scene/ui/animation/AbstractUIAnimation.h"

namespace urchin {

    class AbstractUIWidgetAnimation : public AbstractUIAnimation {
        public:
            explicit AbstractUIWidgetAnimation(Widget&);
            ~AbstractUIWidgetAnimation() override = default;

            const Widget& getWidget() const;

        protected:
            void updatePosition(const Point2<float>&) const;
            void updateScale(const Vector2<float>&) const;
            void updateRotation(float) const;
            void updateAlphaFactor(float) const;

        private:
            Widget& widget;
    };

}