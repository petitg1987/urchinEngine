#ifndef URCHINENGINE_FILELOGGER_H
#define URCHINENGINE_FILELOGGER_H

#include <string>

#include "tools/logger/Logger.h"

#define DEFAULT_LOG_FILENAME "urchinEngine.log"

namespace urchin
{
	
	class FileLogger : public Logger
	{
		public:
			explicit FileLogger(std::string filename = DEFAULT_LOG_FILENAME);

		private:
			void write(const std::string &) override;
			std::string filename;
	};
	
}

#endif
