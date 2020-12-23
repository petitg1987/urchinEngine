#ifndef LASERRIDDLE_SYSTEMINFO_H
#define LASERRIDDLE_SYSTEMINFO_H

#include <string>

namespace urchin {

    class SystemInfo {
        public:
            std::string systemInfo() const;
            std::string systemHash() const;

        private:
            std::string retrieveOsInfo() const;

            unsigned int retrieveCpuCores() const;
            std::string retrieveCpuHash() const;

            unsigned long retrieveTotalMemory() const;

            std::string retrieveGraphicsCardNames() const;
    };

}

#endif
