#include <thread>

#include <system/thread/SleepUtil.h>

namespace urchin {

    /**
     * Sleep the thread for a specified time.
     * Warning: this method is very CPU intensive for OS which does not offer precise sleep method
     */
    void SleepUtil::preciseSleep(double waitTimeUs) {
        #ifdef _WIN32
            auto startWaitTime = std::chrono::steady_clock::now();
            while (true) {
                auto endWaitTime = std::chrono::steady_clock::now();
                auto waitedTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(endWaitTime - startWaitTime).count();
                if ((double)waitedTimeUs >= waitTimeUs) {
                    break;
                }
            }
        #else
            std::this_thread::sleep_for(std::chrono::microseconds(MathFunction::roundToInt(waitTimeUs)));
        #endif
    }

}