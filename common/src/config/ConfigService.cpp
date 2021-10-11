#include <stdexcept>

#include <config/ConfigService.h>
#include <config/FileSystem.h>
#include <io/file/PropertyFileHandler.h>
#include <util/TypeConverter.h>

namespace urchin {

    void ConfigService::loadProperties(const std::string& propertiesFile) {
        loadProperties(propertiesFile, FileSystem::instance().getResourcesDirectory());
    }

    /**
     * @param workingDirectory Override the default working directory
     */
    void ConfigService::loadProperties(const std::string& propertiesFile, const std::string& workingDirectory) {
        std::string propertiesFilePath = workingDirectory + propertiesFile;
        PropertyFileHandler propertyFileHandler(propertiesFilePath);
        properties = propertyFileHandler.loadPropertyFile();

        //build specific maps for performance reason (numeric conversion is slow)
        for (const auto& property : properties) {
            if (TypeConverter::isUnsignedInt(property.second)) {
                unsignedIntProperties[property.first] = TypeConverter::toUnsignedInt(property.second);
            }
            if (TypeConverter::isFloat(property.second)) {
                floatProperties[property.first] = TypeConverter::toFloat(property.second);
            }
        }
    }

    bool ConfigService::isExist(const std::string& propertyName) const {
        auto it = properties.find(propertyName);
        return it != properties.end();
    }

    unsigned int ConfigService::getUnsignedIntValue(const std::string& propertyName) const {
        auto it = unsignedIntProperties.find(propertyName);
        if (it != unsignedIntProperties.end()) {
            return it->second;
        }

        throw std::invalid_argument("The property " + propertyName + " doesn't exist.");
    }

    float ConfigService::getFloatValue(const std::string& propertyName) const {
        auto it = floatProperties.find(propertyName);
        if (it != floatProperties.end()) {
            return it->second;
        }

        throw std::invalid_argument("The property "+ propertyName + " doesn't exist.");
    }

    std::string ConfigService::getStringValue(const std::string& propertyName) const {
        auto it = properties.find(propertyName);
        if (it != properties.end()) {
            return it->second;
        }

        throw std::invalid_argument("The property " + propertyName + " doesn't exist.");
    }

    char ConfigService::getCharValue(const std::string& propertyName) const {
        auto it = properties.find(propertyName);
        if (it != properties.end()) {
            return TypeConverter::toChar(it->second);
        }

        throw std::invalid_argument("The property " + propertyName + " doesn't exist.");
    }

    bool ConfigService::getBoolValue(const std::string& propertyName) const {
        auto it = properties.find(propertyName);
        if (it != properties.end()) {
            bool value = false;
            if (it->second == "true") {
                value = true;
            }
            return value;
        }

        throw std::invalid_argument("The property "+ propertyName + " doesn't exist.");
    }

}
