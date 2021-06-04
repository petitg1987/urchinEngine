#pragma once

#include <string>
#include <map>

#include <pattern/singleton/Singleton.h>

namespace urchin {

    /**
    * Service to handle configuration properties files
    */
    class ConfigService : public Singleton<ConfigService> {
        public:
            friend class Singleton<ConfigService>;

            void loadProperties(const std::string&, const std::map<std::string, std::string> &placeholder={});
            void loadProperties(const std::string&, const std::string&, const std::map<std::string, std::string> &placeholders={});

            bool isExist(const std::string&) const;

            unsigned getUnsignedIntValue(const std::string&) const;
            float getFloatValue(const std::string&) const;
            std::string getStringValue(const std::string&) const;
            char getCharValue(const std::string&) const;
            bool getBoolValue(const std::string&) const;

        private:
            ConfigService();
            ~ConfigService() override = default;

            std::map<std::string, std::string> properties;
            std::map<std::string, float> floatProperties;
            std::map<std::string, unsigned int> unsignedIntProperties;
    };
}
