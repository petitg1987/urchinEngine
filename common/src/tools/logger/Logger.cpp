#include <ctime>
#include <stdexcept>

#include "tools/logger/Logger.h"
#include "tools/logger/FileLogger.h"

namespace urchin {

    //static
    std::unique_ptr<Logger> Logger::customInstance = nullptr;

    Logger::Logger() :
            bHasFailure(false) {

    }

    const std::unique_ptr<Logger>& Logger::defaultInstance() {
        static std::unique_ptr<Logger> defaultInstance = std::make_unique<FileLogger>("urchinEngine.log");
        return defaultInstance;
    }

    void Logger::setupCustomInstance(std::unique_ptr<Logger> logger) {
        customInstance = std::move(logger);
    }

    const std::unique_ptr<Logger>& Logger::instance() {
        if(customInstance) {
            return customInstance;
        }
        return defaultInstance();
    }

    void Logger::logInfo(const std::string& toLog) {
        log(INFO, toLog);
    }

    void Logger::logWarning(const std::string& toLog) {
        log(WARNING, toLog);
    }

    void Logger::logError(const std::string& toLog) {
        log(ERROR, toLog);
    }

    void Logger::log(CriticalityLevel criticalityLevel, const std::string& toLog) {
        #ifndef NDEBUG
            write(prefix(criticalityLevel) + toLog + "\n");
        #else
            if (criticalityLevel >= WARNING) {
                write(prefix(criticalityLevel) + toLog + "\n");
            }
        #endif

        if (criticalityLevel >= WARNING) {
            bHasFailure = true;
        }
    }

    bool Logger::hasFailure() const {
        return bHasFailure;
    }

    /**
     * @return Prefix composed of date/time and criticality
     */
    std::string Logger::prefix(CriticalityLevel criticalityLevel) {
        time_t now = time(nullptr);
        struct tm tstruct = *localtime(&now);
        char buf[80];
        strftime(buf, sizeof(buf), "[%Y-%m-%d %X]", &tstruct);

        std::string result(buf);
        result = result.append(" (");
        result = result.append(getCriticalityString(criticalityLevel));
        result = result.append(") ");

        return result;
    }

    std::string Logger::getCriticalityString(CriticalityLevel criticalityLevel) {
        if (criticalityLevel == INFO) {
            return "II";
        }
        if (criticalityLevel == WARNING) {
            return "WW";
        }
        if (criticalityLevel == ERROR) {
            return "EE";
        }

        throw std::invalid_argument("Unknown logger criticality level.");
    }

}
