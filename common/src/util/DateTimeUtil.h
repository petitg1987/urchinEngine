#pragma once

#include <string>
#include <mutex>
#include <chrono>

namespace urchin {

    class DateTimeUtil {
        public:
            static long currentEpoch();
            static std::string timePointToDateTime(std::chrono::system_clock::time_point);
            static std::string epochToDateTime(long);

        private:
            static std::mutex localtimeMutex;
    };

}