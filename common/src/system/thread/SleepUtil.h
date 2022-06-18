#pragma once

#include <chrono>
#include <thread>

#include <math/algorithm/MathFunction.h>

namespace urchin {

    class SleepUtil {
        public:
            template<class T> static bool stepSleep(int, const T*);

            static void sleepUs(unsigned int);

        private:
            static constexpr int SLEEP_STEP_TIME_MS = 50;
    };

    #include "SleepUtil.inl"

}
