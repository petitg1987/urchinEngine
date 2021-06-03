#ifndef URCHINENGINE_XMLPARSER_H
#define URCHINENGINE_XMLPARSER_H

#include <string>
#include <memory>
#include <vector>

#include <libs/tinyxml/tinyxml.h>
#include <io/xml/XmlAttribute.h>
#include <io/xml/XmlChunk.h>

namespace urchin {

    /**
    * XML parser
    */
    class XmlParser {
        public:
            explicit XmlParser(const std::string&);
            XmlParser(const std::string&, const std::string&);
            ~XmlParser();

            std::shared_ptr<XmlChunk> getRootChunk() const;
            std::vector<std::shared_ptr<XmlChunk>> getChunks(const std::string&, const XmlAttribute & = XmlAttribute(), const std::shared_ptr<XmlChunk> &parent = std::shared_ptr<XmlChunk>()) const;
            std::shared_ptr<XmlChunk> getUniqueChunk(bool, const std::string&, const XmlAttribute & = XmlAttribute(), const std::shared_ptr<XmlChunk> &parent = std::shared_ptr<XmlChunk>()) const;

        private:
            static std::string getChunkDescription(const std::string&, const XmlAttribute&);

            TiXmlDocument* doc;
            std::string filenamePath;
    };

}

#endif
