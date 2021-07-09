#include <stdexcept>

#include <io/data/DataParser.h>
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
        this->doc = std::make_unique<TiXmlDocument>(filenamePath.c_str());

        if (!doc->LoadFile()) {
            throw std::invalid_argument("Cannot open or load the file " + filenamePath + ".");
        }
    }

    std::unique_ptr<DataChunk> DataParser::getRootChunk() const {
        const TiXmlNode* rootNode = doc->FirstChild()->NextSibling();
        if (rootNode->Type() == TiXmlNode::ELEMENT) {
            return std::unique_ptr<DataChunk>(new DataChunk(doc->FirstChild()->NextSibling()->ToElement()));
        }

        throw std::invalid_argument("Impossible to get root element in file: " + filenamePath);
    }

    std::vector<std::unique_ptr<DataChunk>> DataParser::getChunks(const std::string& chunkName, const DataAttribute& attribute, const DataChunk* parent) const {
        std::vector<std::unique_ptr<DataChunk>> chunks;

        const TiXmlNode *firstChild;
        if (!parent) {
            firstChild = doc->FirstChild()->NextSibling()->FirstChild();
        } else {
            firstChild = parent->getChunk()->FirstChild();
        }

        //seek the good chunk
        for (const TiXmlNode *pChild = firstChild; pChild != nullptr; pChild = pChild->NextSibling()) {
            if (pChild->Type() == TiXmlNode::ELEMENT && pChild->Value() == chunkName) {
                if (!attribute.getAttributeName().empty()) {
                    const std::string* attributeValue = pChild->ToElement()->Attribute(attribute.getAttributeName());
                    if (attributeValue && (*attributeValue) == attribute.getAttributeValue()) {
                        chunks.push_back(std::unique_ptr<DataChunk>(new DataChunk(pChild->ToElement())));
                    }
                } else {
                    chunks.push_back(std::unique_ptr<DataChunk>(new DataChunk(pChild->ToElement())));
                }
            }
        }

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
