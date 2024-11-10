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

        std::ifstream file(propertiesFilePath, std::ios::in);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + propertiesFilePath);
        }

        do {
            std::string line;
            FileReader::nextLine(file, line);

            StringUtil::ltrim(line);

            if (line.empty() || line[0] == '#') {
                continue; //ignore commented line
            }

            std::string::size_type equalPosition = line.find('=');
            if (equalPosition == std::string::npos) {
                continue;
            }

            std::string  propertyName = line.substr(0, equalPosition);
            StringUtil::trim(propertyName);

            std::string propertyValue = line.substr(equalPosition + 1);
            StringUtil::trim(propertyValue);
            StringUtil::replaceAll(propertyValue, "\\n", "\n");
            if (!propertyValue.empty() && propertyValue[0] == '"') { //quoted value
                std::size_t endQuotePosition = 0;
                for (std::size_t i = 1; i < propertyValue.size(); ++i) {
                    if (propertyValue[i] == '"') {
                        if (i < propertyValue.size() - 1 && propertyValue[i + 1] == '"') {
                            ++i; //quote escaped with double quote
                        } else {
                            endQuotePosition = i;
                            break;
                        }
                    }
                }
                propertyValue = propertyValue.substr(1, (endQuotePosition == 0) ? 0 : endQuotePosition - 1);
                StringUtil::replaceAll(propertyValue, "\"\"", "\"");
            }

            properties[propertyName] = propertyValue;
        } while (!file.eof());

        return properties;
    }

    void PropertyFileHandler::savePropertyFile(const std::map<std::string, std::string, std::less<>>& properties) const {
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
