#pragma once

#include <functional>

namespace urchin {

    class HashUtil {
        public:
            template <typename... Args> static void combine(std::size_t&, const Args&...);
    };

    #include "HashUtil.inl"

}