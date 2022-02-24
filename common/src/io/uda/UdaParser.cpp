#include <memory>
#include <stdexcept>
#include <regex>
#include <utility>
#include <cassert>

#include <io/uda/UdaParser.h>
#include <io/file/FileReader.h>
#include <config/FileSystem.h>
#include <util/StringUtil.h>

namespace urchin {

    UdaParser::UdaParser(std::string filenamePath) :
            filenamePath(std::move(filenamePath)) {
        std::ifstream file(this->filenamePath, std::ios::in);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + this->filenamePath);
        }
        loadFile(file);
    }

    std::vector<UdaChunk*> UdaParser::getChunks(std::string_view chunkName, const UdaAttribute& attribute, const UdaChunk* parent) const {
        std::vector<UdaChunk*> chunks;

        const auto& nodes = parent ? parent->getChildren() : rootNodes;
        for (const auto& node : nodes) {
            if (isNodeMatchCriteria(*node, chunkName, attribute)) {
                chunks.push_back(node.get());
            }
        }

        return chunks;
    }

    UdaChunk* UdaParser::getFirstChunk(bool mandatory, std::string_view chunkName, const UdaAttribute& attribute, const UdaChunk* parent) const {
        const auto& nodes = parent ? parent->getChildren() : rootNodes;
        for (const auto& node : nodes) {
            if (isNodeMatchCriteria(*node, chunkName, attribute)) {
                return node.get();
            }
        }

        if (mandatory) {
            throw std::invalid_argument("The chunk " + getChunkDescription(std::string(chunkName), attribute) + " was not found in file " + filenamePath);
        }
        return nullptr;
    }

    bool UdaParser::isNodeMatchCriteria(const UdaChunk& node, std::string_view chunkName, const UdaAttribute& attribute) const {
        if (chunkName.empty() || chunkName == node.getName()) {
            if (!attribute.getAttributeName().empty()) {
                std::string attributeValue = node.getAttributeValue(attribute.getAttributeName());
                if (attributeValue == attribute.getAttributeValue()) {
                    return true;
                }
            } else {
                return true;
            }
        }
        return false;
    }

    std::string UdaParser::getChunkDescription(const std::string& chunkName, const UdaAttribute& attribute) const {
        if (attribute.getAttributeName().empty()) {
            return chunkName;
        } else {
            return chunkName + " with the attribute " + attribute.getAttributeName() + "=\"" + attribute.getAttributeValue() + "\"";
        }
    }

    void UdaParser::loadFile(std::ifstream& file) {
        UdaChunk* currentNode = nullptr;
        unsigned int currentNodeIndentLevel = 0;
        while (true) {
            std::string rawContentLine;

            FileReader::nextLine(file, rawContentLine);
            if (rawContentLine.empty()) {
                break;
            }

            unsigned int indentLevel = computeIndentLevel(rawContentLine);
            StringUtil::ltrim(rawContentLine);

            if (rawContentLine.empty() || rawContentLine[0] == '#') {
                continue; //ignore comment line
            }

            if (indentLevel == 0) {
                rootNodes.push_back(buildChunk(rawContentLine, nullptr));
                currentNode = rootNodes.back().get();
                currentNodeIndentLevel = 0;
            } else {
                assert(currentNode);
                if (indentLevel - 1 == currentNodeIndentLevel) {
                    auto newNode = buildChunk(rawContentLine, currentNode);
                    currentNode = &currentNode->addChild(std::move(newNode));
                    currentNodeIndentLevel = indentLevel;
                } else if (indentLevel <= currentNodeIndentLevel) {
                    auto parentNode = currentNode->getParent();
                    for (unsigned int i = 0; i < currentNodeIndentLevel - indentLevel; ++i) {
                        parentNode = parentNode->getParent();
                    }
                    auto newNode = buildChunk(rawContentLine, parentNode);
                    currentNode = &parentNode->addChild(std::move(newNode));
                    currentNodeIndentLevel = indentLevel;
                } else {
                    throw std::runtime_error("Content line (" + rawContentLine +") has wrong indentation in the file: " + filenamePath);
                }
            }
        }
    }

    unsigned int UdaParser::computeIndentLevel(std::string_view lineContent) const {
        unsigned int numberSpaces = 0;
        for (const char& c : lineContent) {
            if (!std::isspace(c)) {
                break;
            }
            numberSpaces++;
        }
        if (numberSpaces % UdaChunk::INDENT_SPACES != 0) {
            throw std::runtime_error("Line content (" + std::string(lineContent) +") with wrong indentation in file: " + filenamePath);
        }
        return numberSpaces / UdaChunk::INDENT_SPACES;
    }

    std::unique_ptr<UdaChunk> UdaParser::buildChunk(const std::string& rawContentLine, UdaChunk* parent) const {
        std::string wrongFormatError = "Content line (" + rawContentLine +") has wrong format in file: " + filenamePath;

        static std::regex parseLineRegex("^" + std::string(NAME_REGEX) + " ?" + std::string(ATTRIBUTES_REGEX) + ": ?" + std::string(VALUE_REGEX) + "$");
        std::smatch matches;
        if (!std::regex_search(rawContentLine, matches, parseLineRegex) || matches.size() != 4) {
            throw std::runtime_error(wrongFormatError);
        }

        std::string name = matches[1].str();
        std::string value = matches[3].str();
        std::map<std::string, std::string, std::less<>> attributes;

        std::string attributesString = matches[2].str();
        if (!attributesString.empty()) {
            std::vector<std::string> attributesVector = StringUtil::split(attributesString, UdaChunk::ATTRIBUTES_SEPARATOR);

            for (const auto& attribute: attributesVector) {
                std::vector<std::string> attributeComponents = StringUtil::split(attribute, UdaChunk::ATTRIBUTES_ASSIGN);
                if (attributeComponents.size() != 2) {
                    throw std::runtime_error(wrongFormatError);
                }
                attributes.try_emplace(attributeComponents[0], attributeComponents[1]);
            }
        }
        return std::make_unique<UdaChunk>(name, value, attributes, parent);
    }

}
