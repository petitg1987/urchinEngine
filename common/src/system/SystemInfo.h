#ifndef URCHINENGINE_SYSTEMINFO_H
#define URCHINENGINE_SYSTEMINFO_H

#include <string>

namespace urchin {

    class SystemInfo {
        public:
            SystemInfo() = delete;

            static std::string retrieveOsInfo();
            static unsigned int retrieveCpuCores();
            static uint64_t retrieveTotalMemory();
            static std::string retrieveGraphicsCardNames();

            static std::string systemHash();

            static std::string homeDirectory();
            static std::string userDataDirectory();
            static std::string executableDirectory();

        private:
            static std::string retrieveCpuHash();

            static std::string getEnvVariable(const std::string&);
    };

}

#endif
