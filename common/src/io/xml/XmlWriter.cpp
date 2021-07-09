#include <stdexcept>

#include <io/xml/XmlWriter.h>
#include <config/FileSystem.h>

namespace urchin {

    XmlWriter::XmlWriter(const std::string& filename) :
            doc(new TiXmlDocument()),
            filenamePath(FileSystem::instance()->getResourcesDirectory() + filename) {
        auto* decl = new TiXmlDeclaration("1.0", "", "");
        doc->LinkEndChild(decl);
    }

    XmlWriter::~XmlWriter() {
        delete doc;
    }

    /**
     * Create a XML chunk according to the parameters (info: <parent><chunkName attributeName="attributeValue">value</chunkName></parent>)
     * @param chunkName Name of the tag XML
     * @param attribute Name and value of the attribute
     * @param parent Name of the tag parent of "chunkName"
     * @return XML chunk according to the parameters
     */
    std::unique_ptr<XmlChunk> XmlWriter::createChunk(const std::string& chunkName, const XmlAttribute& attribute, const XmlChunk* parent) {
        auto* chunk = new TiXmlElement(chunkName);

        if (!attribute.getAttributeName().empty()) {
            chunk->SetAttribute(attribute.getAttributeName(), attribute.getAttributeValue());
        }

        if (parent) {
            parent->getChunk()->LinkEndChild(chunk);
        } else {
            doc->LinkEndChild(chunk);
        }

        return std::unique_ptr<XmlChunk>(new XmlChunk(chunk));
    }

    void XmlWriter::saveInFile() {
        if (!doc->SaveFile(filenamePath)) {
            throw std::invalid_argument("Cannot save the file " + filenamePath + ".");
        }
    }

}
