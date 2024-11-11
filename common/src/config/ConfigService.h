#pragma once

#include <string>
#include <map>

namespace urchin {

    /**
    * Service to handle configuration properties files
    */
    class ConfigService {
        public:
            static ConfigService& instance();

            void loadProperties(const std::string&);
            void loadProperties(const std::string&, const std::string&);

            bool isExist(std::string_view) const;

            unsigned int getUnsignedIntValue(std::string_view) const;
            float getFloatValue(std::string_view) const;
            const std::string& getStringValue(std::string_view) const;
            char getCharValue(std::string_view) const;
            bool getBoolValue(std::string_view) const;

        private:
            ConfigService() = default;

            std::map<std::string, std::string, std::less<>> properties;
            std::map<std::string, float, std::less<>> floatProperties;
            std::map<std::string, unsigned int, std::less<>> unsignedIntProperties;
    };
}
