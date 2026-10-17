#pragma once

#include <optional>
#include <UrchinCommon.h>

namespace urchin {

    enum class VirtualKeyboardType {
        SINGLE_LINE,
        MULTIPLE_LINES
    };

    class VirtualKeyboard {
        public:
            virtual ~VirtualKeyboard() = default;

            virtual void show(VirtualKeyboardType, const std::optional<Rectangle2D<int>>&) = 0;
            virtual void hide() = 0;
    };
}
