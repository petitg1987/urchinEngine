#ifndef LASERRIDDLE_SYSTEMINFO_H
#define LASERRIDDLE_SYSTEMINFO_H

#include <string>

#include "pattern/singleton/Singleton.h"

namespace urchin {

    class SystemInfo : public Singleton<SystemInfo> {
        public:
            friend class Singleton<SystemInfo>;

            std::string retrieveOsInfo() const;
            unsigned int retrieveCpuCores() const;
            unsigned long retrieveTotalMemory() const;
            std::string retrieveGraphicsCardNames() const;

            std::string systemHash() const;

        private:
            SystemInfo() = default;
            ~SystemInfo() override = default;

            std::string retrieveCpuHash() const;
    };

}

#endif
