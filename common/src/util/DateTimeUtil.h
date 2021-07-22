#pragma once

#include <ctime>
#include <string>
#include <mutex>

namespace urchin {

    class DateTimeUtil {
        public:
            static std::string epochToDateTime(time_t);

        private:
            static std::mutex localtimeMutex;
    };

}