#pragma once

#include <string>
#include <memory>

#include <libs/tinyxml/tinyxml.h>
#include <io/data/DataAttribute.h>
#include <io/data/DataChunk.h>

namespace urchin {

    class DataWriter {
        public:
            explicit DataWriter(const std::string&);

            std::unique_ptr<DataChunk> createChunk(const std::string&, const DataAttribute & = DataAttribute(), const DataChunk* parent = nullptr);
            void saveInFile();

        private:
            std::unique_ptr<TiXmlDocument> doc;
            std::string filenamePath;
    };

}
