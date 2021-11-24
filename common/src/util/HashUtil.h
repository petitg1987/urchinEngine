#pragma once

#include <functional>

namespace urchin {

    class HashUtil {
        public:
            template <typename T, typename... Rest> static void combine(std::size_t&, const T&, Rest...);

        private:
            static void combine(const std::size_t&);
    };

    #include "HashUtil.inl"

}