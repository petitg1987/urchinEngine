#include <stdexcept>

#include <io/data/DataParser.h>
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

    void DataParser::loadFile(std::ifstream& file) {
        DataContentLine* currentNode = nullptr;
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
                root = DataContentLine::fromRawContentLine(rawContentLine, nullptr, filenamePath);
                currentNode = root.get();
                currentNodeIndentLevel = 0;
            } else {
                StringUtil::ltrim(rawContentLine);
                if (indentLevel - 1 == currentNodeIndentLevel) {
                    auto newNode = DataContentLine::fromRawContentLine(rawContentLine, currentNode, filenamePath);
                    currentNode = &currentNode->addChild(std::move(newNode));
                    currentNodeIndentLevel = indentLevel;
                } else if (indentLevel <= currentNodeIndentLevel) {
                    auto parentNode = currentNode->getParent();
                    for (unsigned int i = 0; i < currentNodeIndentLevel - indentLevel; ++i) {
                        parentNode = parentNode->getParent();
                    }
                    auto newNode = DataContentLine::fromRawContentLine(rawContentLine, parentNode, filenamePath);
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
        if (numberSpaces % DataChunk::INDENT_SPACES != 0) {
            throw std::runtime_error("Line content (" + lineContent +") with wrong indentation in file: " + filenamePath);
        }
        return numberSpaces / DataChunk::INDENT_SPACES;
    }

    std::unique_ptr<DataChunk> DataParser::getRootChunk() const {
        return std::unique_ptr<DataChunk>(new DataChunk(*root));
    }

    std::vector<std::unique_ptr<DataChunk>> DataParser::getChunks(const std::string& chunkName, const DataAttribute& attribute, const DataChunk* parent) const {
        std::vector<std::unique_ptr<DataChunk>> chunks;

        const DataContentLine *dataContentLine;
        if (!parent) {
            dataContentLine = root.get();
        } else {
            dataContentLine = &parent->chunk;
        }

        //seek the correct chunks
        for (auto& child : dataContentLine->getChildren()) {
            if (child->getName() == chunkName) {
                if (!attribute.getAttributeName().empty()) {
                    std::string attributeValue = child->getAttributes().at(attribute.getAttributeName());
                    if (attributeValue == attribute.getAttributeValue()) {
                        chunks.push_back(std::unique_ptr<DataChunk>(new DataChunk(*child)));
                    }
                } else {
                    chunks.push_back(std::unique_ptr<DataChunk>(new DataChunk(*child)));
                }
            }
        }

        return chunks;
    }

    std::unique_ptr<DataChunk> DataParser::getUniqueChunk(bool mandatory, const std::string& chunkName, const DataAttribute& attribute, const DataChunk* parent) const {
        auto chunks = getChunks(chunkName, attribute, parent);

        if (chunks.size() > 1) {
            throw std::invalid_argument("More than one chunk found for " + getChunkDescription(chunkName, attribute));
        } else if (chunks.empty()) {
            if (mandatory) {
                throw std::invalid_argument("The chunk " + getChunkDescription(chunkName, attribute) + " was not found in file " + filenamePath);
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
