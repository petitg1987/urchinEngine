#ifndef URCHINENGINE_FILELOGGER_H
#define URCHINENGINE_FILELOGGER_H

#include <string>

#include "tools/logger/Logger.h"

namespace urchin
{
	
	class FileLogger : public Logger
	{
		public:
			explicit FileLogger(std::string);

			std::string readAll() const;

		private:
			void write(const std::string &) override;

			std::string filename;
	};
	
}

#endif
