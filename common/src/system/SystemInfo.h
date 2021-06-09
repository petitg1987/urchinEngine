#pragma once

#include <string>

namespace urchin {

    class SystemInfo {
        public:
            enum KeyboardLayout {
                DEFAULT,
                AZERTY
            };

            SystemInfo() = delete;

            static std::string retrieveOsInfo();
            static unsigned int retrieveCpuCores();
            static uint64_t retrieveTotalMemory();
            static std::string retrieveGraphicsCardNames();

            static std::string systemHash();

            static std::string homeDirectory();
            static std::string userDataDirectory();
            static std::string executableDirectory();

            static KeyboardLayout keyboardLayout();

        private:
            static std::string retrieveCpuHash();

            static std::string getEnvVariable(const std::string&);
    };

}
