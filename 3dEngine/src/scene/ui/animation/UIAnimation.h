#pragma once

#include <scene/ui/widget/Widget.h>

namespace urchin {

    class UIAnimation {
        public:
            explicit UIAnimation(Widget&);
            virtual ~UIAnimation() = default;

            void animate(float);
            virtual bool isCompleted() const = 0;

        protected:
            virtual void initializeAnimation() = 0;
            virtual void doAnimation(float) = 0;

            const Widget& getWidget() const;
            void updatePosition(const Point2<float>&);

        private:
            Widget& widget;
            bool animationInitialized;
    };

}