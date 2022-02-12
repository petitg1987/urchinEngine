#include <iostream>
#include <stdexcept>
#include <utility>

#include <io/file/PropertyFileHandler.h>
#include <io/file/FileReader.h>
#include <util/StringUtil.h>

namespace urchin {

    PropertyFileHandler::PropertyFileHandler(std::string propertiesFilePath) :
            propertiesFilePath(std::move(propertiesFilePath)) {

    }

    std::map<std::string, std::string, std::less<>> PropertyFileHandler::loadPropertyFile() const {
        std::map<std::string, std::string, std::less<>> properties;

        std::string propertyName;
        std::string equalSign;
        std::string propertyValue;

        std::ifstream file(propertiesFilePath, std::ios::in);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + propertiesFilePath);
        }

        do {
            std::string line;
            FileReader::nextLine(file, line);

            std::string::size_type equalPosition = line.find('=');
            if (equalPosition == std::string::npos) {
                continue;
            }

            propertyName = line.substr(0, equalPosition);
            StringUtil::trim(propertyName);

            propertyValue = line.substr(equalPosition + 1);
            StringUtil::trim(propertyValue);
            StringUtil::replaceAll(propertyValue, "\\n", "\n");
            if (propertyValue.size() > 1 && propertyValue[0] == '"' && propertyValue[propertyValue.size() - 1] == '"') { //quoted string value
                propertyValue = propertyValue.substr(1, propertyValue.size() - 2);
            }

            properties[propertyName] = propertyValue;
        } while (!file.eof());

        return properties;
    }

    void PropertyFileHandler::savePropertyFile(const std::map<std::string, std::string>& properties) const {
        std::ofstream file;
        file.open(propertiesFilePath, std::ios::out);
        if (file.fail()) {
            throw std::invalid_argument("Cannot open the file " + propertiesFilePath + ".");
        }

        for (const auto& [propertyName, propertyValue] : properties) {
            file << propertyName << " = " << propertyValue <<"\n";
        }

        file.close();
    }

}
