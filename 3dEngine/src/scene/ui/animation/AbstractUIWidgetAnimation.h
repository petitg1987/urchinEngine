#pragma once

#include <scene/ui/widget/Widget.h>
#include <scene/ui/animation/AbstractUIAnimation.h>

namespace urchin {

    class AbstractUIWidgetAnimation : public AbstractUIAnimation {
        public:
            explicit AbstractUIWidgetAnimation(Widget&);
            ~AbstractUIWidgetAnimation() override = default;

        protected:
            const Widget& getWidget() const;
            void updatePosition(const Point2<float>&);
            void updateScale(const Vector2<float>&);
            void updateRotation(float);

        private:
            Widget& widget;
    };

}