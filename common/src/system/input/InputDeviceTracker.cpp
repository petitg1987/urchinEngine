#include <cmath>

#include "system/input/InputDeviceTracker.h"
#include "system/input/InputKeyNavigation.h"

namespace urchin {

    InputDeviceTracker::InputDeviceTracker() :
            gamepadType(GamepadType::OTHER),
            lastInputDevice(InputDevice::KEYBOARD_MOUSE) {

    }

    void InputDeviceTracker::onGamepadTypeUpdate(GamepadType gamepadType) {
        this->gamepadType = gamepadType;
    }

    void InputDeviceTracker::onKeyPress(InputKey key) {
        if (key != InputKey::UNKNOWN_KEY) {
            lastInputDevice = InputKeyNavigation::isGamepadKey(key) ? toGamepadInputDevice() : InputDevice::KEYBOARD_MOUSE;
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
            lastInputDevice = toGamepadInputDevice();
        }
    }

    bool InputDeviceTracker::isGamepad(InputDevice inputDevice) {
        return inputDevice != InputDevice::KEYBOARD_MOUSE;
    }

    InputDevice InputDeviceTracker::getLastInputDevice() const {
        return lastInputDevice;
    }

    InputDevice InputDeviceTracker::toGamepadInputDevice() const {
        if (gamepadType == GamepadType::XBOX) {
            return InputDevice::GAMEPAD_XBOX;
        } else if (gamepadType == GamepadType::PLAYSTATION) {
            return InputDevice::GAMEPAD_PLAYSTATION;
        }
        return InputDevice::GAMEPAD_OTHER;
    }

}
