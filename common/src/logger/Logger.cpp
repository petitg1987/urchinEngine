#include <stdexcept>

#include <logger/Logger.h>
#include <logger/FileLogger.h>
#include <util/DateTimeUtil.h>

namespace urchin {

    //static
    std::unique_ptr<Logger> Logger::customInstance = nullptr;

    Logger::Logger() :
            bHasError(false),
            bHasWarning(false) {

    }

    Logger& Logger::defaultInstance() {
        static std::unique_ptr<Logger> defaultInstance = std::make_unique<FileLogger>("urchinEngine.log");
        return *defaultInstance;
    }

    void Logger::setupCustomInstance(std::unique_ptr<Logger> logger) {
        customInstance = std::move(logger);
    }

    Logger& Logger::instance() {
        if (customInstance) {
            return *customInstance;
        }
        return defaultInstance();
    }

    void Logger::logInfo(const std::string& toLog) {
        log(INFO_LVL, toLog);
    }

    void Logger::logWarning(const std::string& toLog) {
        log(WARNING_LVL, toLog);
    }

    void Logger::logError(const std::string& toLog) {
        log(ERROR_LVL, toLog);
    }

    void Logger::log(CriticalityLevel criticalityLevel, const std::string& toLog) {
        write(prefix(criticalityLevel) + toLog + "\n");

        if (criticalityLevel == ERROR_LVL) {
            bHasError = true;
        } else if (criticalityLevel == WARNING_LVL) {
            bHasWarning = true;
        }
    }

    bool Logger::hasError() const {
        return bHasError;
    }

    bool Logger::hasWarningOrError() const {
        return bHasWarning || bHasError;
    }

    /**
     * @return Prefix composed of date/time and criticality
     */
    std::string Logger::prefix(CriticalityLevel criticalityLevel) const {
        std::string result = "[" + DateTimeUtil::timePointToDateTime(std::chrono::system_clock::now(), DateTimeUtil::DATE_HOUR_MIN_SEC) + "]";
        result += " (" + getCriticalityString(criticalityLevel) + ") ";
        return result;
    }

    std::string Logger::getCriticalityString(CriticalityLevel criticalityLevel) const {
        if (criticalityLevel == INFO_LVL) {
            return "II";
        } else if (criticalityLevel == WARNING_LVL) {
            return "WW";
        } else if (criticalityLevel == ERROR_LVL) {
            return "EE";
        }

        throw std::invalid_argument("Unknown logger criticality level.");
    }

}
