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
		log(INFO, toLog);
	}

	void Logger::logWarning(const std::string &toLog)
	{
		log(WARNING, toLog);
	}

	void Logger::logError(const std::string &toLog)
	{
		log(ERROR, toLog);
	}

	void Logger::log(CriticalityLevel criticalityLevel, const std::string &toLog)
	{
		#ifdef _DEBUG
			write(prefix(criticalityLevel) + toLog + "\n");
		#else
			if(criticalityLevel >= WARNING)
			{
				write(prefix(criticalityLevel) + toLog + "\n");
			}
		#endif
	}

	/**
	 * @return Prefix composed of date/time and criticality
	 */
	std::string Logger::prefix(CriticalityLevel criticalityLevel)
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

	std::string Logger::getCriticalityString(CriticalityLevel criticalityLevel)
	{
		if(criticalityLevel == INFO)
		{
			return "II";
		}else if(criticalityLevel == WARNING)
		{
			return "WW";
		}else if(criticalityLevel == ERROR)
		{
			return "EE";
		}

		throw std::invalid_argument("Unknown logger criticality level.");
	}

}
