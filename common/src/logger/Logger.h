#pragma once

#include <string>
#include <sstream>
#include <memory>
#include <iostream>

namespace urchin {

    class Logger {
        public:
            Logger();
            virtual ~Logger() = default;

            enum CriticalityLevel {
                INFO_LVL,
                WARNING_LVL,
                ERROR_LVL
            };

            static void setupCustomInstance(std::unique_ptr<Logger>);
            static const std::unique_ptr<Logger>& instance();

            void logInfo(const std::string&);
            void logWarning(const std::string&);
            void logError(const std::string&);
            void log(CriticalityLevel, const std::string&);

            virtual std::string retrieveContent(unsigned long) const = 0;
            virtual std::string retrieveTarget() const = 0;
            virtual void purge() const = 0;
            virtual std::string archive() const = 0;

            bool hasFailure() const;

        private:
            static const std::unique_ptr<Logger>& defaultInstance();
            static std::string prefix(CriticalityLevel);
            static std::string getCriticalityString(CriticalityLevel);

            virtual void write(const std::string&) = 0;

            bool bHasFailure;
            static std::unique_ptr<Logger> customInstance;
    };

}
