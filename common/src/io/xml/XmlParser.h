#pragma once

#include <string>
#include <memory>
#include <vector>

#include <libs/tinyxml/tinyxml.h>
#include <io/xml/XmlAttribute.h>
#include <io/xml/XmlChunk.h>

namespace urchin {

    class XmlParser {
        public:
            explicit XmlParser(const std::string&);
            XmlParser(const std::string&, const std::string&);
            ~XmlParser();

            std::unique_ptr<XmlChunk> getRootChunk() const;
            std::vector<std::unique_ptr<XmlChunk>> getChunks(const std::string&, const XmlAttribute & = XmlAttribute(), const XmlChunk *parent = nullptr) const;
            std::unique_ptr<XmlChunk> getUniqueChunk(bool, const std::string&, const XmlAttribute & = XmlAttribute(), const XmlChunk *parent = nullptr) const;

        private:
            std::string getChunkDescription(const std::string&, const XmlAttribute&) const;

            TiXmlDocument* doc;
            std::string filenamePath;
    };

}
