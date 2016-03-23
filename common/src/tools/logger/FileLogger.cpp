#include <fstream>
#include <stdexcept>

#include "tools/logger/FileLogger.h"

namespace urchin
{
	
	FileLogger::FileLogger(const std::string &filename) : Logger()
	{
		this->filename = filename;
	}

	void FileLogger::write(const std::string &msg)
	{
		std::ofstream file;
		file.open(filename, std::ios::app);
		if(file.fail())
		{
			throw std::invalid_argument("Cannot open the file " + filename + ".");
		}
		file.write(msg.c_str(), msg.length());
		file.close();
	}

}
