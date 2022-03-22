#pragma once

#include <AL/alc.h>
#include <string_view>

class CheckState {
    public:
        static void check(std::string_view);
        static void checkContext(ALCdevice*, std::string_view);

    private:
        static constexpr unsigned int MAX_ERRORS_LOG = 50;
};
