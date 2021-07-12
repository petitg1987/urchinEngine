#include <regex>
#include <iostream>

#include <io/data/model/DataContentLine.h>
#include <util/StringUtil.h>

namespace urchin {

    DataContentLine::DataContentLine(const std::string& lineContent, DataContentLine* parent):
            parent(parent) {
        std::string nameRegex = "([a-zA-Z]+)";
        std::string attributesRegex = "\\(?(.*?)\\)?";
        std::string valueRegex = "\"?(.*)\"?";
        std::regex lineRegex(nameRegex + " ?" + attributesRegex + ": ?" + valueRegex + "$"); //TODO static

        std::smatch matches;
        if (std::regex_search(lineContent, matches, lineRegex)) {
            if (matches.size() != 4) {
                throw std::runtime_error("Wrong formatted line: " + lineContent);
            }

            name = matches[1].str();
            value = matches[3].str();

            std::string attributesString = matches[2].str();
            if (!attributesString.empty()) {
                std::vector<std::string> attributesVector;
                StringUtil::split(attributesString, ';', attributesVector);

                for (auto &attribute: attributesVector) {
                    std::vector<std::string> attributeComponents;
                    StringUtil::split(attribute, '=', attributeComponents);
                    if(attributeComponents.size() != 2) {
                        throw std::runtime_error("Wrong formatted line to load attributes: " + lineContent); //TODO missing filename !
                    }

                    attributes.emplace(attributeComponents[0], attributeComponents[1]);
                }
            }
        } else {
            throw std::runtime_error("Wrong formatted line: " + lineContent); //TODO missing filename !
        }
    }

    DataContentLine* DataContentLine::getParent() const {
        return parent;
    }

    void DataContentLine::addChild(std::unique_ptr<DataContentLine> child) {
        children.push_back(std::move(child));
    }

    const std::string& DataContentLine::getName() const {
        return name;
    }

    const std::string& DataContentLine::getValue() const {
        return value;
    }

    const std::map<std::string, std::string>& DataContentLine::getAttributes() const {
        return attributes;
    }
}
