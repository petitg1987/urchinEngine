#pragma once

#include "system/input/InputKey.h"

namespace urchin {

    class InputKeyNavigation {
        public:
            static bool isUp(InputKey);
            static bool isDown(InputKey);
            static bool isLeft(InputKey);
            static bool isRight(InputKey);

            static bool isBack(InputKey);
            static bool isGamepadKey(InputKey);
    };

}
