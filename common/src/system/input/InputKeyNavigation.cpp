#include "system/input/InputKeyNavigation.h"

namespace urchin {

    bool InputKeyNavigation::isUp(InputKey key) {
        return key == InputKey::ARROW_UP || key == InputKey::GAMEPAD_DPAD_UP || key == InputKey::GAMEPAD_LEFT_STICK_UP;
    }

    bool InputKeyNavigation::isDown(InputKey key) {
        return key == InputKey::ARROW_DOWN || key == InputKey::GAMEPAD_DPAD_DOWN || key == InputKey::GAMEPAD_LEFT_STICK_DOWN;
    }

    bool InputKeyNavigation::isLeft(InputKey key) {
        return key == InputKey::ARROW_LEFT || key == InputKey::GAMEPAD_DPAD_LEFT || key == InputKey::GAMEPAD_LEFT_STICK_LEFT;
    }

    bool InputKeyNavigation::isRight(InputKey key) {
        return key == InputKey::ARROW_RIGHT || key == InputKey::GAMEPAD_DPAD_RIGHT || key == InputKey::GAMEPAD_LEFT_STICK_RIGHT;
    }

    bool InputKeyNavigation::isValidate(InputKey key) {
        return key == InputKey::ENTER || key == InputKey::NUM_PAD_ENTER || key == InputKey::GAMEPAD_BUTTON_SOUTH;
    }

    bool InputKeyNavigation::isBack(InputKey key) {
        return key == InputKey::GAMEPAD_BUTTON_EAST;
    }

    bool InputKeyNavigation::isGamepadKey(InputKey key) {
        return key >= InputKey::GAMEPAD_BUTTON_SOUTH && key <= InputKey::GAMEPAD_LEFT_STICK_RIGHT;
    }

}
