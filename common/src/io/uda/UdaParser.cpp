#include <memory>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <ranges>
#include <stack>

#include <io/uda/UdaParser.h>
#include <io/file/FileReader.h>
#include <config/FileSystem.h>
#include <util/StringUtil.h>

namespace urchin {

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
        auto newNodePtr = newNode.get();
        if (!parent) {
            rootNodes.push_back(std::move(newNode));
        } else {
            parent->addChild(std::move(newNode));
        }

        return *newNodePtr;
    }

    void UdaParser::removeChunk(UdaChunk& chunk) {
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
        for (const auto& rootNode : std::ranges::reverse_view(rootNodes)) {
            stack.push(rootNode.get());
        }
        while (!stack.empty()) {
            const UdaChunk* node = stack.top();
            stack.pop();

            unsigned int indentLevel = computeIndentLevel(*node);
            std::string indent(indentLevel * UdaChunk::INDENT_SPACES, ' ');

            file << indent << buildRawContentLine(*node) << std::endl;

            const auto& children = node->getChildren();
            for (const auto& it : std::ranges::reverse_view(children)) {
                stack.push(it.get());
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
        std::string rawAttributes;
        if (!udaChunk.getAttributes().empty()) {
            rawAttributes.append(" (");
            auto& attributes = udaChunk.getAttributes();
            for (auto it = attributes.begin(); it != attributes.end(); ++it) {
                if (it != attributes.begin()) {
                    rawAttributes += UdaChunk::ATTRIBUTES_SEPARATOR;
                }
                rawAttributes.append(it->first);
                rawAttributes += UdaChunk::ATTRIBUTES_ASSIGN;
                rawAttributes.append(it->second);
            }
            rawAttributes.append(")");
        }

        std::string rawValue;
        if (!udaChunk.getStringValue().empty()) {
            rawValue = " \"" + udaChunk.getStringValue() + "\"";
        }

        return udaChunk.getName() + rawAttributes + ":" + rawValue;
    }

}
