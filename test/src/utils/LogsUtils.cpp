#include <fstream>
#include <sstream>

#include "utils/LogsUtils.h"

bool LogsUtils::logFileContain(const std::string &str)
{
    std::ifstream logFile(LOG_FILENAME);
    std::stringstream buffer;
    buffer << logFile.rdbuf();

    return buffer.str().find(str) != std::string::npos;
}

void LogsUtils::emptyLogFile()
{
    std::ofstream ofs;
    ofs.open(LOG_FILENAME, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}
