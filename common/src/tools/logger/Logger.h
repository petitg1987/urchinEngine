#ifndef URCHINENGINE_LOGGER_H
#define URCHINENGINE_LOGGER_H

#include <string>
#include <sstream>
#include <memory>
#include <iostream>

namespace urchin
{

	class Logger
	{
		public:
            Logger();
			virtual ~Logger() = default;

			enum CriticalityLevel
			{
				INFO,
				WARNING,
				ERROR
			};

            static std::unique_ptr<Logger> defineLogger(std::unique_ptr<Logger>);
			static Logger& logger();

			void logInfo(const std::string &);
			void logWarning(const std::string &);
			void logError(const std::string &);
			void log(CriticalityLevel, const std::string &);

			bool hasFailure();

		private:
			std::string prefix(CriticalityLevel);
			std::string getCriticalityString(CriticalityLevel);

			virtual void write(const std::string &) = 0;

            bool bHasFailure;
			static std::unique_ptr<Logger> instance;
	};

}

#endif
