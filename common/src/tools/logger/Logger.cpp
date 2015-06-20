#include <time.h>
#include <stdexcept>

#include "tools/logger/Logger.h"

namespace urchin
{
	
	//static
	Logger *Logger::instance = nullptr;

	Logger::Logger()
	{

	}

	Logger::~Logger()
	{

	}

	Logger& Logger::logger()
	{
		return *instance;
	}

	void Logger::setLogger(Logger *newLogger)
	{
		delete instance;
		instance = newLogger;
	}

	/**
	 * @return Prefix composed of date/time and criticality
	 */
	std::string Logger::prefix(LoggerCriticalityLevel criticalityLevel)
	{
	    time_t now = time(0);
	    struct tm tstruct;
	    char buf[80];
	    tstruct = *localtime(&now);
	    strftime(buf, sizeof(buf), "[%Y-%m-%d %X]", &tstruct);

	    std::string result(buf);
	    result = result.append(" (");
	    result = result.append(getCriticalityString(criticalityLevel));
	    result = result.append(") ");

	    return result;
	}

	std::string Logger::getCriticalityString(LoggerCriticalityLevel criticalityLevel)
	{
		if(criticalityLevel == LOG_INFO)
		{
			return "II";
		}else if(criticalityLevel == LOG_WARNING)
		{
			return "WW";
		}else if(criticalityLevel == LOG_ERROR)
		{
			return "EE";
		}

		throw std::invalid_argument("Unknown logger criticality level.");
	}

}
