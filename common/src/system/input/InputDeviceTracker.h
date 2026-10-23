#pragma once

#include "system/input/InputKey.h"

namespace urchin {

    enum class GamepadType {
        XBOX,
        PLAYSTATION,
        OTHER
    };

    enum class InputDevice {
        KEYBOARD_MOUSE,
        GAMEPAD_XBOX,
        GAMEPAD_PLAYSTATION,
        GAMEPAD_OTHER
    };

    class InputDeviceTracker {
        public:
            InputDeviceTracker();

            void onGamepadTypeUpdate(GamepadType);
            void onKeyPress(InputKey);
            void onMouseMove(double, double);
            void onScroll();
            void onGamepadAnalogInputUpdate(float);

            static bool isGamepad(InputDevice);
            InputDevice getLastInputDevice() const;

        private:
            InputDevice toGamepadInputDevice() const;

            static constexpr float GAMEPAD_ANALOG_INPUT_THRESHOLD = 0.25f;

            GamepadType gamepadType;
            InputDevice lastInputDevice;
    };

}
