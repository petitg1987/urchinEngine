#pragma once

#include <string>
#include <map>

#include <pattern/singleton/ThreadSafeSingleton.h>

namespace urchin {

    /**
    * Service to handle configuration properties files
    */
    class ConfigService : public ThreadSafeSingleton<ConfigService> {
        public:
            friend class ThreadSafeSingleton<ConfigService>;

            void loadProperties(const std::string&);
            void loadProperties(const std::string&, const std::string&);

            bool isExist(const std::string&) const;

            unsigned getUnsignedIntValue(const std::string&) const;
            float getFloatValue(const std::string&) const;
            std::string getStringValue(const std::string&) const;
            char getCharValue(const std::string&) const;
            bool getBoolValue(const std::string&) const;

        private:
            ConfigService() = default;

            std::map<std::string, std::string> properties;
            std::map<std::string, float> floatProperties;
            std::map<std::string, unsigned int> unsignedIntProperties;
    };
}
