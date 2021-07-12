#include <stdexcept>

#include <io/data/DataWriter.h>
#include <config/FileSystem.h>

namespace urchin {

    DataWriter::DataWriter(const std::string& filename) :
            doc(std::make_unique<TiXmlDocument>()),
            filenamePath(FileSystem::instance()->getResourcesDirectory() + filename) {
        auto* decl = new TiXmlDeclaration("1.0", "", "");
        doc->LinkEndChild(decl);
    }

    std::unique_ptr<DataChunk> DataWriter::createChunk(const std::string& chunkName, const DataAttribute& attribute, const DataChunk* /*parent*/) {
        auto* chunk = new TiXmlElement(chunkName);

        if (!attribute.getAttributeName().empty()) {
            chunk->SetAttribute(attribute.getAttributeName(), attribute.getAttributeValue());
        }

//        if (parent) {
//            parent->getChunk()->LinkEndChild(chunk);
//        } else {
//            doc->LinkEndChild(chunk);
//        }

        return std::unique_ptr<DataChunk>(nullptr/*new DataChunk(chunk)*/);
    }

    void DataWriter::saveInFile() {
        if (!doc->SaveFile(filenamePath)) {
            throw std::invalid_argument("Cannot save the file " + filenamePath + ".");
        }
    }

}
