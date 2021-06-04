#pragma once

#include <string>
#include <memory>

#include <libs/tinyxml/tinyxml.h>
#include <io/xml/XmlAttribute.h>
#include <io/xml/XmlChunk.h>

namespace urchin {

    /**
    * XML writer
    */
    class XmlWriter {
        public:
            explicit XmlWriter(const std::string&);
            ~XmlWriter();

            std::shared_ptr<XmlChunk> createChunk(const std::string&, const XmlAttribute & = XmlAttribute(), const std::shared_ptr<XmlChunk> &parent = std::shared_ptr<XmlChunk>());
            void saveInFile();

        private:
            TiXmlDocument* doc;
            std::string filenamePath;
    };

}
