#ifndef _WIN32
    #include <ctime>
#endif

#include "system/thread/SleepUtil.h"

namespace urchin {

    /**
     * Sleep the thread for a specified time in microseconds.
     * Warning: this method is very CPU intensive for OS which does not offer precise sleep method
     */
    void SleepUtil::sleepUs(long sleepTimeUs) {
        #ifdef _WIN32
            auto startWaitTime = std::chrono::steady_clock::now();
            while (true) {
                auto endWaitTime = std::chrono::steady_clock::now();
                auto waitedTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(endWaitTime - startWaitTime).count();
                if (waitedTimeUs >= sleepTimeUs) {
                    break;
                }
            }
        #else
            timespec timespecVal = {.tv_sec = 0, .tv_nsec = sleepTimeUs * 1000};
            nanosleep(&timespecVal, nullptr);
        #endif
    }

}