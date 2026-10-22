#include <cmath>

#include "system/input/InputDeviceTracker.h"
#include "system/input/InputKeyNavigation.h"

namespace urchin {

    InputDeviceTracker::InputDeviceTracker() :
            lastInputDevice(InputDevice::KEYBOARD_MOUSE) {

    }

    void InputDeviceTracker::onKeyPress(InputKey key) {
        if (key != InputKey::UNKNOWN_KEY) {
            lastInputDevice = InputKeyNavigation::isGamepadKey(key) ? InputDevice::GAMEPAD : InputDevice::KEYBOARD_MOUSE;
        }
    }

    void InputDeviceTracker::onMouseMove(double deltaMouseX, double deltaMouseY) {
        if (deltaMouseX != 0.0 || deltaMouseY != 0.0) {
            lastInputDevice = InputDevice::KEYBOARD_MOUSE;
        }
    }

    void InputDeviceTracker::onScroll() {
        lastInputDevice = InputDevice::KEYBOARD_MOUSE;
    }

    void InputDeviceTracker::onGamepadAnalogInputUpdate(float value) {
        if (std::abs(value) >= GAMEPAD_ANALOG_INPUT_THRESHOLD) {
            lastInputDevice = InputDevice::GAMEPAD;
        }
    }

    InputDevice InputDeviceTracker::getLastInputDevice() const {
        return lastInputDevice;
    }

}
