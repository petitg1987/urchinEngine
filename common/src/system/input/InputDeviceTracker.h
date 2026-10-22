#pragma once

#include "system/input/InputKey.h"

namespace urchin {

    enum class InputDevice {
        KEYBOARD_MOUSE,
        GAMEPAD
    };

    class InputDeviceTracker {
        public:
            InputDeviceTracker();

            void onKeyPress(InputKey);
            void onMouseMove(double, double);
            void onScroll();
            void onGamepadAnalogInputUpdate(float);

            InputDevice getLastInputDevice() const;

        private:
            static constexpr float GAMEPAD_ANALOG_INPUT_THRESHOLD = 0.25f;

            InputDevice lastInputDevice;
    };

}
