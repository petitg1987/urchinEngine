#include <time.h>
#include <stdexcept>

#include "tools/logger/Logger.h"
#include "tools/logger/FileLogger.h"

namespace urchin
{
	
	//static
	Logger *Logger::instance = new FileLogger();

	Logger::Logger()
	{

	}

	Logger::~Logger()
	{
		delete instance;
		instance = nullptr;
	}

	Logger& Logger::logger()
	{
		return *instance;
	}

	void Logger::logInfo(const std::string &toLog)
	{
		log(LOG_INFO, toLog);
	}

	void Logger::logWarning(const std::string &toLog)
	{
		log(LOG_WARNING, toLog);
	}

	void Logger::logError(const std::string &toLog)
	{
		log(LOG_ERROR, toLog);
	}

	void Logger::log(LoggerCriticalityLevel criticalityLevel, const std::string &toLog)
	{
		#ifdef _DEBUG
			write(prefix(criticalityLevel) + toLog + "\n");
		#else
			if(criticalityLevel >= LOG_ERROR)
			{
				write(prefix(criticalityLevel) + toLog + "\n");
			}
		#endif
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
