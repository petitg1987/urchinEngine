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

			static void setLogger(Logger *);
			
			static Logger& logger();
			template<class T> Logger& operator <<(const T& toLog);

			static std::string prefix(LoggerCriticalityLevel);
		private:
			virtual void write(const std::string &) = 0;
			static std::string getCriticalityString(LoggerCriticalityLevel);

			static Logger *instance;
	};

	#include "Logger.inl"

}

#endif
