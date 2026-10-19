#include <algorithm>
#include <cassert>

#include "system/input/GamepadAnalogStick.h"

namespace urchin {

    GamepadAnalogStick::GamepadAnalogStick(float deadZone) :
            deadZone(deadZone),
            rawValue(Vector2(0.0f, 0.0f)) {
        assert(deadZone >= 0.0f && deadZone < 1.0f);
    }

    /**
     * Axis values are in [-1, 1] range: X positive is right
     */
    void GamepadAnalogStick::setX(float x) {
        rawValue.X = std::clamp(x, -1.0f, 1.0f);
    }

    /**
     * Axis values are in [-1, 1] range: Y positive is down.
     */
    void GamepadAnalogStick::setY(float y) {
        rawValue.Y = std::clamp(y, -1.0f, 1.0f);
    }

    void GamepadAnalogStick::reset() {
        rawValue.setNull();
    }

    Vector2<float> GamepadAnalogStick::getDirection() const {
        float length = rawValue.length();
        if (length <= deadZone) {
            return Vector2(0.0f, 0.0f);
        }
        float rescaledLength = std::min((length - deadZone) / (1.0f - deadZone), 1.0f);
        return rawValue * (rescaledLength / length);
    }

}
