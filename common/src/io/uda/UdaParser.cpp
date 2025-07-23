#include <memory>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <ranges>
#include <stack>

#include "io/uda/UdaParser.h"
#include "io/file/FileReader.h"
#include "util/StringUtil.h"

namespace urchin {

    UdaParser::UdaParser(std::vector<std::unique_ptr<UdaChunk>> rootNodes) :
            rootNodes(std::move(rootNodes)) {

    }

    UdaParser::UdaParser(std::string filenamePath, UdaLoadType loadType) :
            filenamePath(std::move(filenamePath)) {
        if (loadType == UdaLoadType::LOAD_FILE) {
            std::ifstream file(this->filenamePath, std::ios::in);
            if (!file.is_open()) {
                throw std::invalid_argument("Unable to open file: " + this->filenamePath);
            }
            loadFile(file);
            file.close();
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
                continue; //ignore commented line
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
        std::string name;
        std::string value;

        bool hasAttributes = false;
        std::string attributesString;
        std::map<std::string, std::string, std::less<>> attributes;

        for (std::size_t charIndex = 0; charIndex < rawContentLine.size(); ++charIndex) {
            if (name.empty()) {
                if (rawContentLine[charIndex] == '(') {
                    hasAttributes = true;
                    name = rawContentLine.substr(0, charIndex - 1);
                } else if (rawContentLine[charIndex] == ':') {
                    name = rawContentLine.substr(0, charIndex);
                }
            } else if (hasAttributes && attributesString.empty()) {
                if (rawContentLine[charIndex] == ')') {
                    attributesString = rawContentLine.substr(name.size() + 2, charIndex - name.size() - 2);
                }
            } else {
                std::size_t valueStartIndex = charIndex + 1 + (hasAttributes ? 2 /* skip: colon & space */ : 1 /* skip: space */);
                std::size_t valueEndIndex = rawContentLine.size() - 1;
                if (valueStartIndex < rawContentLine.size() && valueStartIndex < valueEndIndex) {
                    value = rawContentLine.substr(valueStartIndex, valueEndIndex - valueStartIndex);
                }
                break;
            }
        }

        if (!attributesString.empty()) {
            std::vector<std::string> attributesVector = StringUtil::split(attributesString, UdaChunk::ATTRIBUTES_SEPARATOR);

            for (const auto& attribute: attributesVector) {
                std::vector<std::string> attributeComponents = StringUtil::split(attribute, UdaChunk::ATTRIBUTES_ASSIGN);
                if (attributeComponents.size() != 2) {
                    throw std::runtime_error("Content line (" + rawContentLine +") has wrong format in file: " + filenamePath);
                }
                attributes.try_emplace(attributeComponents[0], attributeComponents[1]);
            }
        }
        return std::make_unique<UdaChunk>(name, value, attributes, parent);
    }

    UdaChunk& UdaParser::createChunk(std::string chunkName, const UdaAttribute& attribute, UdaChunk* parent) {
        std::map<std::string, std::string, std::less<>> attributes;
        if (!attribute.getAttributeName().empty()) {
            attributes.try_emplace(attribute.getAttributeName(), attribute.getAttributeValue());
        }

        auto newNode = std::make_unique<UdaChunk>(std::move(chunkName), "", attributes, parent);
        if (!parent) {
            rootNodes.push_back(std::move(newNode));
            return *rootNodes.back();
        } else {
            return parent->addChild(std::move(newNode));
        }
    }

    UdaChunk& UdaParser::addChunk(const UdaChunk& chunkToCopy, UdaChunk* parent) {
        auto newNode = std::make_unique<UdaChunk>(chunkToCopy);
        if (!parent) {
            rootNodes.push_back(std::move(newNode));
            return *rootNodes.back();
        } else {
            return parent->addChild(std::move(newNode));
        }
    }

    void UdaParser::removeChunk(const UdaChunk& chunk) {
        if (chunk.getParent()) {
            chunk.getParent()->removeChild(chunk);
        } else {
            std::erase_if(rootNodes, [&chunk](const std::unique_ptr<UdaChunk> &node){ return &chunk == node.get(); });
        }
    }

    void UdaParser::save() const {
        std::ofstream file;
        file.open (filenamePath);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + filenamePath);
        }

        std::stack<UdaChunk*> stack;
        for (const auto& rootNode : rootNodes | std::views::reverse) {
            stack.push(rootNode.get());
        }
        while (!stack.empty()) {
            const UdaChunk* node = stack.top();
            stack.pop();

            unsigned int indentLevel = computeIndentLevel(*node);
            std::string indent((std::size_t)indentLevel * UdaChunk::INDENT_SPACES, ' ');

            file << indent << buildRawContentLine(*node) << '\n';

            for (const auto& child : node->getChildren() | std::views::reverse) {
                stack.push(child.get());
            }
        }

        file.close();
    }

    unsigned int UdaParser::computeIndentLevel(const UdaChunk& udaChunk) const {
        unsigned int indentLevel = 0;
        const auto* parentNode = udaChunk.getParent();
        while (parentNode != nullptr) {
            parentNode = parentNode->getParent();
            indentLevel++;
        }
        return indentLevel;
    }

    std::string UdaParser::buildRawContentLine(const UdaChunk& udaChunk) const {
        std::string rawContentLine = udaChunk.getName();
        rawContentLine.reserve(50); //estimated memory size

        if (!udaChunk.getAttributes().empty()) {
            rawContentLine.append(" (");
            auto& attributes = udaChunk.getAttributes();
            for (auto it = attributes.begin(); it != attributes.end(); ++it) {
                if (it != attributes.begin()) {
                    rawContentLine += UdaChunk::ATTRIBUTES_SEPARATOR;
                }
                rawContentLine.append(it->first);
                rawContentLine += UdaChunk::ATTRIBUTES_ASSIGN;
                rawContentLine.append(it->second);
            }
            rawContentLine.append("):");
        } else {
            rawContentLine.append(":");
        }

        if (!udaChunk.getStringValue().empty()) {
            rawContentLine.append(" \"");
            rawContentLine.append(udaChunk.getStringValue());
            rawContentLine.append("\"");
        }

        return rawContentLine;
    }

}
