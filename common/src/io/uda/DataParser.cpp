#include <memory>
#include <stdexcept>
#include <regex>

#include <io/uda/DataParser.h>
#include <io/file/FileReader.h>
#include <config/FileSystem.h>
#include <util/StringUtil.h>

namespace urchin {

    DataParser::DataParser(const std::string& filename) :
            DataParser(filename, FileSystem::instance()->getResourcesDirectory()) {

    }

    /**
     * @param workingDirectory Override the default working directory
     */
    DataParser::DataParser(const std::string& filename, const std::string& workingDirectory) {
        filenamePath = workingDirectory + filename;

        std::ifstream file(filenamePath, std::ios::in);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + filenamePath);
        }
        loadFile(file);
    }

    UdaChunk* DataParser::getRootChunk() const {
        return root.get();
    }

    std::vector<UdaChunk*> DataParser::getChunks(const std::string& chunkName, const UdaAttribute& attribute, const UdaChunk* parent) const {
        std::vector<UdaChunk*> chunks;
        const UdaChunk *udaChunk = parent ? parent : root.get();

        for (auto& child : udaChunk->getChildren()) {
            if (child->getName() == chunkName) {
                if (!attribute.getAttributeName().empty()) {
                    std::string attributeValue = child->getAttributeValue(attribute.getAttributeName());
                    if (attributeValue == attribute.getAttributeValue()) {
                        chunks.push_back(child.get());
                    }
                } else {
                    chunks.push_back(child.get());
                }
            }
        }

        return chunks;
    }

    UdaChunk* DataParser::getUniqueChunk(bool mandatory, const std::string& chunkName, const UdaAttribute& attribute, const UdaChunk* parent) const {
        auto chunks = getChunks(chunkName, attribute, parent);

        if (chunks.size() > 1) {
            throw std::invalid_argument("More than one chunk found for " + getChunkDescription(chunkName, attribute));
        } else if (chunks.empty()) {
            if (mandatory) {
                throw std::invalid_argument("The chunk " + getChunkDescription(chunkName, attribute) + " was not found in file " + filenamePath);
            }
            return nullptr;
        }
        return chunks[0];
    }

    std::string DataParser::getChunkDescription(const std::string& chunkName, const UdaAttribute& attribute) const {
        if (attribute.getAttributeName().empty()) {
            return chunkName;
        } else {
            return chunkName + " with the attribute " + attribute.getAttributeName() + "=\"" + attribute.getAttributeValue() + "\"";
        }
    }

    void DataParser::loadFile(std::ifstream& file) {
        UdaChunk* currentNode = nullptr;
        unsigned int currentNodeIndentLevel = 0;
        while (true) {
            std::string rawContentLine;

            FileReader::nextLine(file, rawContentLine);
            if (rawContentLine.empty()) {
                break;
            }

            unsigned int indentLevel = computeIndentLevel(rawContentLine);
            if (indentLevel == 0) {
                if (root) {
                    throw std::runtime_error("Content line (" + rawContentLine +") has wrong indentation in the file: " + filenamePath);
                }
                root = buildChunk(rawContentLine, nullptr);
                currentNode = root.get();
                currentNodeIndentLevel = 0;
            } else {
                StringUtil::ltrim(rawContentLine);
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

    unsigned int DataParser::computeIndentLevel(const std::string& lineContent) const {
        unsigned int numberSpaces = 0;
        for (const char& c : lineContent) {
            if (!std::isspace(c)) {
                break;
            }
            numberSpaces++;
        }
        if (numberSpaces % UdaChunk::INDENT_SPACES != 0) {
            throw std::runtime_error("Line content (" + lineContent +") with wrong indentation in file: " + filenamePath);
        }
        return numberSpaces / UdaChunk::INDENT_SPACES;
    }

    std::unique_ptr<UdaChunk> DataParser::buildChunk(const std::string& rawContentLine, UdaChunk* parent) const {
        std::string wrongFormatError = "Content line (" + rawContentLine +") has wrong format in file: " + filenamePath;

        static std::regex parseLineRegex("^" + std::string(NAME_REGEX) + " ?" + std::string(ATTRIBUTES_REGEX) + ": ?" + std::string(VALUE_REGEX) + "$");
        std::smatch matches;
        if (!std::regex_search(rawContentLine, matches, parseLineRegex) || matches.size() != 4) {
            throw std::runtime_error(wrongFormatError);
        }

        std::string name = matches[1].str();
        std::string value = matches[3].str();
        std::map<std::string, std::string> attributes;

        std::string attributesString = matches[2].str();
        if (!attributesString.empty()) {
            std::vector<std::string> attributesVector;
            StringUtil::split(attributesString, UdaChunk::ATTRIBUTES_SEPARATOR, attributesVector);

            for (auto &attribute: attributesVector) {
                std::vector<std::string> attributeComponents;
                StringUtil::split(attribute, UdaChunk::ATTRIBUTES_ASSIGN, attributeComponents);
                if (attributeComponents.size() != 2) {
                    throw std::runtime_error(wrongFormatError);
                }
                attributes.emplace(attributeComponents[0], attributeComponents[1]);
            }
        }
        return std::make_unique<UdaChunk>(name, value, attributes, parent);
    }

}
