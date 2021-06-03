#include <ctime>
#include <stdexcept>

#include <logger/Logger.h>
#include <logger/FileLogger.h>

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
        if (customInstance) {
            return customInstance;
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

        if (criticalityLevel >= WARNING_LVL) {
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
        struct tm timeStruct = *localtime(&now);
        char buffer[64];
        strftime(buffer, sizeof(buffer), "[%Y-%m-%d %X]", &timeStruct);

        std::string result(buffer);
        result += " (" + getCriticalityString(criticalityLevel) + ") ";
        return result;
    }

    std::string Logger::getCriticalityString(CriticalityLevel criticalityLevel) {
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
