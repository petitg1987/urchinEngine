#include <stdexcept>
#include <fstream>

#include <io/data/DataParser.h>
#include <io/file/FileReader.h>
#include <config/FileSystem.h>

namespace urchin {

    DataParser::DataParser(const std::string& filename) :
            DataParser(filename, FileSystem::instance()->getResourcesDirectory()) {

    }

    /**
     * @param workingDirectory Override the default working directory
     */
    DataParser::DataParser(const std::string& filename, const std::string& workingDirectory) {
        this->filenamePath = workingDirectory + filename;

        std::ifstream file(filenamePath, std::ios::in);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + filenamePath);
        }

        std::string lineContent;
        DataContentLine* currentNode = nullptr;
        unsigned int currentNodeIndentLevel = 0;
        while (true) {
            FileReader::nextLine(file, lineContent);
            if (lineContent.empty()) {
                break;
            }

            unsigned int indentLevel = computeIndentLevel(lineContent);
            if (indentLevel == 0) {
                assert(!root);
                root = std::make_unique<DataContentLine>(lineContent, nullptr);
                currentNode = root.get();
                currentNodeIndentLevel = 0;
            } else {
                if (indentLevel - 1 == currentNodeIndentLevel) {
                    auto newNode = std::make_unique<DataContentLine>(lineContent, currentNode);
                    auto* newNodePtr = newNode.get();
                    currentNode->addChild(std::move(newNode));

                    currentNode = newNodePtr;
                    currentNodeIndentLevel = indentLevel;
                } else if (indentLevel <= currentNodeIndentLevel) {
                    unsigned int diffLevel = currentNodeIndentLevel - indentLevel;
                    auto parentNode = currentNode->getParent();
                    for(unsigned int i = 0; i < diffLevel; ++i) {
                        parentNode = parentNode->getParent();
                    }
                    auto newNode = std::make_unique<DataContentLine>(lineContent, parentNode);
                    auto* newNodePtr = newNode.get();
                    parentNode->addChild(std::move(newNode));

                    currentNode = newNodePtr;
                    currentNodeIndentLevel = indentLevel;
                } else {
                    throw std::runtime_error("Line content (" + lineContent +") with wrong indentation in file: " + filenamePath);
                }
            }
        }
    }

    unsigned int DataParser::computeIndentLevel(const std::string& lineContent) {
        unsigned int numberSpaces = 0;
        for (const char& c : lineContent) {
            if (c != ' ') {
                break;
            }
            numberSpaces++;
        }
        if (numberSpaces % 2 != 0) {
            throw std::runtime_error("Line content (" + lineContent +") with wrong indentation in file: " + filenamePath);
        }
        return numberSpaces / 2;
    }

    std::unique_ptr<DataChunk> DataParser::getRootChunk() const {
        //TODO ...
        throw std::invalid_argument("Impossible to get root element in file: " + filenamePath);
    }

    std::vector<std::unique_ptr<DataChunk>> DataParser::getChunks(const std::string& /*chunkName*/, const DataAttribute& /*attribute*/, const DataChunk* /*parent*/) const {
        std::vector<std::unique_ptr<DataChunk>> chunks;

        //TODO ...

        return chunks;
    }

    std::unique_ptr<DataChunk> DataParser::getUniqueChunk(bool mandatory, const std::string& chunkName, const DataAttribute& attribute, const DataChunk* parent) const {
        auto chunks = getChunks(chunkName, attribute, parent);

        if (chunks.size() > 1) {
            throw std::invalid_argument("More than one tag found for " + getChunkDescription(chunkName, attribute) + ".");
        } else if (chunks.empty()) {
            if (mandatory) {
                throw std::invalid_argument("The tag " + getChunkDescription(chunkName, attribute) + " wasn't found in file " + filenamePath + ".");
            }

            return std::unique_ptr<DataChunk>(nullptr);
        }

        return std::move(chunks[0]);
    }

    std::string DataParser::getChunkDescription(const std::string& chunkName, const DataAttribute& attribute) const {
        if (attribute.getAttributeName().empty()) {
            return chunkName;
        } else {
            return chunkName + " with the attribute " + attribute.getAttributeName() + "=\"" + attribute.getAttributeValue() + "\"";
        }
    }

}
