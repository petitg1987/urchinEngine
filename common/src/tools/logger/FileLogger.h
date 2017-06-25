#ifndef ENGINE_FILELOGGER_H
#define ENGINE_FILELOGGER_H

#include <string>

#include "tools/logger/Logger.h"

#define DEFAULT_LOG_FILENAME "urchinEngine.log"

namespace urchin
{
	
	class FileLogger : public Logger
	{
		public:
			FileLogger(std::string filename = DEFAULT_LOG_FILENAME);

		private:
			void write(const std::string &);
			std::string filename;
	};
	
}

#endif
