#pragma once

#include <chrono>

namespace urchin {

    class StepSleep {
        public:
            template<class T> static bool sleep(int, const T*);

        private:
            static constexpr int SLEEP_STEP_TIME_MS = 50;
    };

    #include "StepSleep.inl"

}
