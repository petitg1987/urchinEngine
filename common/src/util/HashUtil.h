#pragma once

#include <functional>
#include <string_view>
#include <cstdint>

namespace urchin {

    class HashUtil {
        public:
            //WARNING: there is no guarantee that the same hash value will be produced across different computers, platforms, or program builds.
            template <typename... Args> static void hashCombine(std::size_t&, const Args&...);

            static uint32_t stableHash(std::string_view);
    };

    #include "HashUtil.inl"

}