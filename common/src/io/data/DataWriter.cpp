#include <stdexcept>

#include <io/data/DataWriter.h>
#include <config/FileSystem.h>

namespace urchin {

    DataWriter::DataWriter(const std::string& filename) :
            filenamePath(FileSystem::instance()->getResourcesDirectory() + filename) {

    }

    std::unique_ptr<DataChunk> DataWriter::createChunk(const std::string& chunkName, const DataAttribute& attribute, const DataChunk* parent) {
        std::map<std::string, std::string> attributes;
        if (!attribute.getAttributeName().empty()) {
            attributes.emplace(attribute.getAttributeName(), attribute.getAttributeValue());
        }

        DataContentLine* nodeParent = parent ? &parent->chunk : nullptr;
        auto newNode = std::make_unique<DataContentLine>(chunkName, "", attributes, nodeParent);
        auto& newNodeRef = *newNode;
        if (!parent) {
            root = std::move(newNode);
        } else {
            parent->chunk.addChild(std::move(newNode));
        }

        return std::unique_ptr<DataChunk>(new DataChunk(newNodeRef));
    }

    void DataWriter::saveInFile() {
        //TODO complete..
        std::cout<<root.get()<<std::endl;
//        if (!doc->SaveFile(filenamePath)) {
//            throw std::invalid_argument("Cannot save the file " + filenamePath + ".");
//        }
    }

}
