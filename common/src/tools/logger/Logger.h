#ifndef URCHINENGINE_LOGGER_H
#define URCHINENGINE_LOGGER_H

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
                INFO,
                WARNING,
                ERROR
            };

            static void setupCustomInstance(std::unique_ptr<Logger>);
            static const std::unique_ptr<Logger>& instance();

            void logInfo(const std::string&);
            void logWarning(const std::string&);
            void logError(const std::string&);
            void log(CriticalityLevel, const std::string&);

            virtual std::string retrieveContent(unsigned long) const = 0;
            virtual void purge() const = 0;
            virtual void archive() const = 0;

            bool hasFailure() const;

        private:
            static const std::unique_ptr<Logger>& defaultInstance();
            std::string prefix(CriticalityLevel);
            std::string getCriticalityString(CriticalityLevel);

            virtual void write(const std::string&) = 0;

            bool bHasFailure;
            static std::unique_ptr<Logger> customInstance;
    };

}

#endif
