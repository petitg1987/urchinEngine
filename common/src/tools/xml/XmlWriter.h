#ifndef URCHINENGINE_XMLWRITER_H
#define URCHINENGINE_XMLWRITER_H

#include <string>
#include <memory>

#include "libs/tinyxml/tinyxml.h"
#include "tools/xml/XmlAttribute.h"
#include "tools/xml/XmlChunk.h"

namespace urchin {

    /**
    * XML writer
    */
    class XmlWriter {
        public:
            explicit XmlWriter(const std::string &);
            ~XmlWriter();

            std::shared_ptr<XmlChunk> createChunk(const std::string &, const XmlAttribute & = XmlAttribute(), const std::shared_ptr<XmlChunk> &parent = std::shared_ptr<XmlChunk>());
            void saveInFile();

        private:
            TiXmlDocument *doc;
            std::string filenamePath;
    };

}

#endif
