#pragma once

#include <string>
#include <memory>
#include <vector>

#include <libs/tinyxml/tinyxml.h>
#include <io/data/DataAttribute.h>
#include <io/data/DataChunk.h>

namespace urchin {

    class DataParser {
        public:
            explicit DataParser(const std::string&);
            DataParser(const std::string&, const std::string&);

            std::unique_ptr<DataChunk> getRootChunk() const;
            std::vector<std::unique_ptr<DataChunk>> getChunks(const std::string&, const DataAttribute& = DataAttribute(), const DataChunk* parent = nullptr) const;
            std::unique_ptr<DataChunk> getUniqueChunk(bool, const std::string&, const DataAttribute& = DataAttribute(), const DataChunk* parent = nullptr) const;

        private:
            std::string getChunkDescription(const std::string&, const DataAttribute&) const;

            std::unique_ptr<TiXmlDocument> doc;
            std::string filenamePath;
    };

}
