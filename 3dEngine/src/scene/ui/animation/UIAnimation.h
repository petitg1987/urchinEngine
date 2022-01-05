#pragma once

namespace urchin {

    class UIAnimation {
        public:
            virtual ~UIAnimation() = default;

            virtual Point2<float> getPosition() const = 0;

            virtual void animate(float) = 0;
    };

}