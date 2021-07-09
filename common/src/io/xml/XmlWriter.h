#pragma once

#include <string>
#include <memory>

#include <libs/tinyxml/tinyxml.h>
#include <io/xml/XmlAttribute.h>
#include <io/xml/XmlChunk.h>

namespace urchin {

    class XmlWriter {
        public:
            explicit XmlWriter(const std::string&);
            ~XmlWriter();

            std::unique_ptr<XmlChunk> createChunk(const std::string&, const XmlAttribute & = XmlAttribute(), const XmlChunk* parent = nullptr);
            void saveInFile();

        private:
            TiXmlDocument* doc;
            std::string filenamePath;
    };

}
