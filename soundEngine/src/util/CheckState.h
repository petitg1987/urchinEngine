#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <string_view>

namespace urchin {

    class CheckState {
        public:
            static void checkContext(ALCdevice*, std::string_view);
            template <typename... Args> static void check(std::string_view, const Args&...);

        private:
            static constexpr unsigned int MAX_ERRORS_LOG = 50;
    };

    #include "CheckState.inl"

}