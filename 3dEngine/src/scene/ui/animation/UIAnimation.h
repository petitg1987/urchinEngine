#pragma once

#include <scene/ui/widget/Widget.h>

namespace urchin {

    class UIAnimation {
        public:
            explicit UIAnimation(Widget&);
            virtual ~UIAnimation() = default;

            virtual void animate(float) = 0;
            virtual bool isCompleted() const = 0;

        protected:
            void updatePosition(const Point2<float>&);

        private:
            Widget& widget;
    };

}