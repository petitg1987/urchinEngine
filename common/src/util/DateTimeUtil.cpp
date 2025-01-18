#include <mutex>
#include <iomanip>

#include <util/DateTimeUtil.h>

namespace urchin {

    //static
    std::mutex DateTimeUtil::localtimeMutex;

    uint64_t DateTimeUtil::currentEpoch() {
        return (uint64_t)std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    std::string DateTimeUtil::timePointToDateTime(std::chrono::system_clock::time_point timePoint, Format format) {
        if (format == DATE_HOUR_MIN_SEC) {
            return timePointToCustomFormat(timePoint, "%Y-%m-%d %X");
        } else if (format == DATE_HOUR_MIN) {
            return timePointToCustomFormat(timePoint, "%Y-%m-%d %H:%M");
        }
        throw std::runtime_error("Unknown requested date time format: " + std::to_string(format));
    }

    std::string DateTimeUtil::timePointToDate(std::chrono::system_clock::time_point timePoint) {
        return timePointToCustomFormat(timePoint, "%Y-%m-%d");
    }

    std::string DateTimeUtil::timePointToCustomFormat(std::chrono::system_clock::time_point timePoint, const std::string& format) {
        std::time_t epochSeconds = std::chrono::system_clock::to_time_t(timePoint);
        std::stringstream ss;
        {
            std::scoped_lock lock(localtimeMutex); //mutex for not thread safe localtime function
            ss << std::put_time(std::localtime(&epochSeconds), format.c_str());
        }
        return ss.str();
    }

    std::string DateTimeUtil::epochToDateTime(uint64_t epochSeconds) {
        auto timePoint = std::chrono::system_clock::time_point{std::chrono::seconds{epochSeconds}};
        return timePointToDateTime(timePoint, DATE_HOUR_MIN_SEC);
    }

    std::string DateTimeUtil::secondsToHhmm(double totalSeconds, char hourSeparator) {
        int hours = static_cast<int>(totalSeconds) / 3600;
        int minutes = (static_cast<int>(totalSeconds) % 3600) / 60;

        std::ostringstream result;
        result << std::setw(2) << std::setfill('0') << hours << hourSeparator
               << std::setw(2) << std::setfill('0') << minutes;
        return result.str();
    }

    std::string DateTimeUtil::secondsToHhmmss(double totalSeconds, char hourSeparator, char minuteSeparator) {
        int hours = static_cast<int>(totalSeconds) / 3600;
        int minutes = (static_cast<int>(totalSeconds) % 3600) / 60;
        int seconds = static_cast<int>(totalSeconds) % 60;

        std::ostringstream result;
        result << std::setw(2) << std::setfill('0') << hours << hourSeparator
               << std::setw(2) << std::setfill('0') << minutes << minuteSeparator
               << std::setw(2) << std::setfill('0') << seconds;
        return result.str();
    }

}