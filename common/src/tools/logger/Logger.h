#ifndef ENGINE_LOGGER_H
#define ENGINE_LOGGER_H

#include <string>
#include <sstream>
#include <iostream>

namespace urchin
{
	
	class Logger
	{
		public:
			Logger();
			virtual ~Logger();

			enum LoggerCriticalityLevel
			{
				LOG_INFO,
				LOG_WARNING,
				LOG_ERROR
			};
			
			static Logger& logger();

			void logInfo(const std::string &);
			void logWarning(const std::string &);
			void logError(const std::string &);

		private:
			void log(LoggerCriticalityLevel, const std::string &);

			std::string prefix(LoggerCriticalityLevel);
			std::string getCriticalityString(LoggerCriticalityLevel);

			virtual void write(const std::string &) = 0;

			static Logger *instance;
	};

}

#endif
