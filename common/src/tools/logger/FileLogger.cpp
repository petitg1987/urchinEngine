#include <fstream>
#include <stdexcept>
#include <vector>

#include "tools/logger/FileLogger.h"

namespace urchin
{
	
	FileLogger::FileLogger(std::string filename) :
		Logger(),
		filename(std::move(filename))
	{

	}

	const std::string FileLogger::getFilename() const
	{
		return filename;
	}

	std::string FileLogger::readAll() const
	{
		std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
		auto fileSize = static_cast<unsigned long>(ifs.tellg());
		ifs.seekg(0, std::ios::beg);

		std::vector<char> bytes(fileSize);
		ifs.read(bytes.data(), fileSize);

		return std::string(bytes.data(), fileSize);
	}

	void FileLogger::clearLogs()
	{
        std::ofstream file;
        file.open(filename, std::ofstream::out | std::ofstream::trunc);
        file.close();
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
		file.flush();
		file.close();
	}

}
