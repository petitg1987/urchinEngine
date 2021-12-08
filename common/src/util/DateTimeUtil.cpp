#include <mutex>
#include <iomanip>

#include <util/DateTimeUtil.h>

namespace urchin {

    //static
    std::mutex DateTimeUtil::localtimeMutex;

    uint64_t DateTimeUtil::currentEpoch() {
        return (uint64_t)std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    std::string DateTimeUtil::timePointToDateTime(std::chrono::system_clock::time_point timePoint) {
        std::time_t epochSeconds = std::chrono::system_clock::to_time_t(timePoint);
        std::stringstream ss;
        {
            std::scoped_lock<std::mutex> lock(localtimeMutex); //mutex for not thread safe localtime function
            ss << std::put_time(std::localtime(&epochSeconds), "%Y-%m-%d %X");
        }
        return ss.str();
    }

    std::string DateTimeUtil::epochToDateTime(uint64_t epochSeconds) {
        auto timePoint = std::chrono::system_clock::time_point{std::chrono::seconds{epochSeconds}};
        return timePointToDateTime(timePoint);
    }

}