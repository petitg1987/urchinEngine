#include <sstream>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "io/file/PropertyFileHandler.h"
#include "io/file/FileReader.h"
#include "util/StringUtil.h"

namespace urchin {

    PropertyFileHandler::PropertyFileHandler(std::string propertiesFilePath) :
            propertiesFilePath(std::move(propertiesFilePath)) {

    }

    std::map<std::string, std::string> PropertyFileHandler::loadPropertyFile() const {
        std::map<std::string, std::string> properties;

        std::string propertyName;
        std::string equalSign;
        std::string propertyValue;

        std::ifstream file(propertiesFilePath, std::ios::in);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + propertiesFilePath);
        }

        do
        {
            std::string buffer;
            FileReader::nextLine(file, buffer);

            std::istringstream iss(buffer);
            iss >> propertyName;

            if (buffer.length() == 0 || propertyName[0] == '#') { //empty or commented line
                continue;
            }

            iss >> equalSign;
            std::getline(iss, propertyValue);
            StringUtil::instance()->ltrim(propertyValue);

            properties[propertyName] = propertyValue;
        }while (!file.eof());

        return properties;
    }

    void PropertyFileHandler::savePropertyFile(const std::map<std::string, std::string>& properties) const {
        std::ofstream file;
        file.open(propertiesFilePath, std::ios::out);
        if (file.fail()) {
            throw std::invalid_argument("Cannot open the file " + propertiesFilePath + ".");
        }

        for (const auto& property : properties) {
            file << property.first << " = " << property.second <<"\n";
        }

        file.close();
    }

}
