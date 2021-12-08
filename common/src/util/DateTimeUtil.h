#pragma once

#include <string>
#include <mutex>
#include <chrono>

namespace urchin {

    class DateTimeUtil {
        public:
            static uint64_t currentEpoch();
            static std::string timePointToDateTime(std::chrono::system_clock::time_point);
            static std::string epochToDateTime(uint64_t);

        private:
            static std::mutex localtimeMutex;
    };

}