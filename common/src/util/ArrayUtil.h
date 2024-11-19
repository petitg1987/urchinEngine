#pragma once

#include <array>

namespace urchin {

    class ArrayUtil {
        public:
            template <typename Type, std::size_t... sizes> static auto concatenate(const std::array<Type, sizes>&...);
    };

    #include "ArrayUtil.inl"

}