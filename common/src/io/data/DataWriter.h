#pragma once

#include <string>
#include <memory>

#include <libs/tinyxml/tinyxml.h>
#include <io/data/DataAttribute.h>
#include <io/data/XmlChunk.h>

namespace urchin {

    class DataWriter {
        public:
            explicit DataWriter(const std::string&);
            ~DataWriter();

            std::unique_ptr<XmlChunk> createChunk(const std::string&, const DataAttribute & = DataAttribute(), const XmlChunk* parent = nullptr);
            void saveInFile();

        private:
            TiXmlDocument* doc;
            std::string filenamePath;
    };

}
