#ifndef URCHINENGINE_LOGSUTILS_H
#define URCHINENGINE_LOGSUTILS_H

#define LOG_FILENAME "urchinEngine.log"

class LogsUtils
{
    public:
        static bool logFileContain(const std::string &);
        static void emptyLogFile();
};

#endif
