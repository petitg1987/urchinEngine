#include <iostream>

#include <io/data/model/DataContentLine.h>
#include <util/StringUtil.h>

namespace urchin {

    DataContentLine::DataContentLine(std::string name, std::string value, std::map<std::string, std::string> attributes, DataContentLine* parent):
            name(std::move(name)),
            value(std::move(value)),
            attributes(std::move(attributes)),
            parent(parent) {

    }

    std::unique_ptr<DataContentLine> DataContentLine::fromRawContentLine(const std::string& lineContent, DataContentLine* parent, const std::string& filename) {
        std::string wrongFormatError = "Line content (" + lineContent +") has wrong format in file: " + filename;

        static std::regex parseLineRegex("^" + std::string(NAME_REGEX) + " ?" + std::string(ATTRIBUTES_REGEX) + ": ?" + std::string(VALUE_REGEX) + "$");
        std::smatch matches;
        if (!std::regex_search(lineContent, matches, parseLineRegex) || matches.size() != 4) {
            throw std::runtime_error(wrongFormatError);
        }

        std::string name = matches[1].str();
        std::string value = matches[3].str();
        std::map<std::string, std::string> attributes;

        std::string attributesString = matches[2].str();
        if (!attributesString.empty()) {
            std::vector<std::string> attributesVector;
            StringUtil::split(attributesString, ATTRIBUTES_SEPARATOR, attributesVector);

            for (auto &attribute: attributesVector) {
                std::vector<std::string> attributeComponents;
                StringUtil::split(attribute, ATTRIBUTES_ASSIGN, attributeComponents);
                if (attributeComponents.size() != 2) {
                    throw std::runtime_error(wrongFormatError);
                }
                attributes.emplace(attributeComponents[0], attributeComponents[1]);
            }
        }
        return std::make_unique<DataContentLine>(name, value, attributes, parent);
    }

    std::string DataContentLine::toRawContentLine(DataContentLine& dataContentLine) {
        std::string rawAttributes;
        if (!dataContentLine.getAttributes().empty()) {
            rawAttributes.append(" (");
            for (const auto& [key, value] : dataContentLine.getAttributes()) {
                if (!rawAttributes.empty()) {
                    rawAttributes += ATTRIBUTES_SEPARATOR;
                }
                rawAttributes.append(key);
                rawAttributes += ATTRIBUTES_ASSIGN;
                rawAttributes.append(value);
            }
            rawAttributes.append(")");
        }

        std::string rawValue;
        if (!dataContentLine.getValue().empty()) {
            rawValue = " \"" + dataContentLine.getValue() + "\"";
        }

        return dataContentLine.getName() + rawAttributes + ":" + rawValue;
    }

    DataContentLine* DataContentLine::getParent() const {
        return parent;
    }

    const std::vector<std::unique_ptr<DataContentLine>>& DataContentLine::getChildren() const {
        return children;
    }

    DataContentLine& DataContentLine::addChild(std::unique_ptr<DataContentLine> child) {
        children.push_back(std::move(child));
        return *children.back();
    }

    const std::string& DataContentLine::getName() const {
        return name;
    }

    void DataContentLine::setValue(const std::string &value) {
        this->value = value;
    }

    const std::string& DataContentLine::getValue() const {
        return value;
    }

    void DataContentLine::addAttribute(const std::string& key, const std::string& value) {
        attributes.emplace(key, value);
    }

    const std::map<std::string, std::string>& DataContentLine::getAttributes() const {
        return attributes;
    }
}
