#pragma once

#include <string>
#include <memory>
#include <vector>

#include <libs/tinyxml/tinyxml.h>
#include <io/data/DataAttribute.h>
#include <io/data/XmlChunk.h>

namespace urchin {

    class DataParser {
        public:
            explicit DataParser(const std::string&);
            DataParser(const std::string&, const std::string&);

            std::unique_ptr<XmlChunk> getRootChunk() const;
            std::vector<std::unique_ptr<XmlChunk>> getChunks(const std::string&, const DataAttribute& = DataAttribute(), const XmlChunk* parent = nullptr) const;
            std::unique_ptr<XmlChunk> getUniqueChunk(bool, const std::string&, const DataAttribute& = DataAttribute(), const XmlChunk* parent = nullptr) const;

        private:
            std::string getChunkDescription(const std::string&, const DataAttribute&) const;

            std::unique_ptr<TiXmlDocument> doc;
            std::string filenamePath;
    };

}
