#pragma once

#include "math/algebra/vector/Vector2.h"

namespace urchin {

    class GamepadAnalogStick {
        public:
            static constexpr float DEFAULT_DEAD_ZONE = 0.15f;

            explicit GamepadAnalogStick(float = DEFAULT_DEAD_ZONE);

            void setX(float);
            void setY(float);
            void reset();

            Vector2<float> getDirection() const;

        private:
            float deadZone;
            Vector2<float> rawValue;
    };

}
