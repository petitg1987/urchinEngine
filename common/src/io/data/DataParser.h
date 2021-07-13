#pragma once

#include <string>
#include <memory>
#include <vector>
#include <fstream>

#include <io/data/DataAttribute.h>
#include <io/data/DataChunk.h>
#include <io/data/model/DataContentLine.h>

namespace urchin {

    class DataParser {
        public:
            explicit DataParser(const std::string&);
            DataParser(const std::string&, const std::string&);

            std::unique_ptr<DataChunk> getRootChunk() const;
            std::vector<std::unique_ptr<DataChunk>> getChunks(const std::string&, const DataAttribute& = DataAttribute(), const DataChunk* parent = nullptr) const;
            std::unique_ptr<DataChunk> getUniqueChunk(bool, const std::string&, const DataAttribute& = DataAttribute(), const DataChunk* parent = nullptr) const;

        private:
            void loadFile(std::ifstream&);
            unsigned int computeIndentLevel(const std::string&) const;

            std::string getChunkDescription(const std::string&, const DataAttribute&) const;

            std::string filenamePath;
            std::unique_ptr<DataContentLine> root;
    };

}
