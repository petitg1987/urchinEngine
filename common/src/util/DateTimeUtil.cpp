#include <mutex>
#include <util/DateTimeUtil.h>

namespace urchin {

    //static
    std::mutex DateTimeUtil::localtimeMutex;

    std::string DateTimeUtil::epochToDateTime(time_t epochSeconds) {
        char timeBuffer[64];
        {
            std::lock_guard<std::mutex> lock(localtimeMutex); //mutex for not thread safe localtime function
            tm timeStruct = *localtime(&epochSeconds);
            strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %X", &timeStruct);
        }

        return std::string(timeBuffer);
    }

}