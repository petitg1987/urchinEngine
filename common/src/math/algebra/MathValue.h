#pragma once

#include <numbers>

namespace urchin {

    class MathValue {
        public:
            static constexpr double PI = std::numbers::pi;
            static constexpr float PI_FLOAT = (float)PI;
    };

}
