#pragma once

#include "system/input/InputKey.h"

namespace urchin {

    /**
     * Classify the keys used to navigate in a user interface without mouse (keyboard or gamepad)
     */
    class InputKeyNavigation {
        public:
            static bool isUp(InputKey);
            static bool isDown(InputKey);
            static bool isLeft(InputKey);
            static bool isRight(InputKey);
            static bool isValidate(InputKey);
            static bool isBack(InputKey);
            static bool isGamepadKey(InputKey);
    };

}
