#pragma once

#include <string>
#include <memory>
#include <vector>
#include <fstream>

#include <io/data/DataAttribute.h>
#include <io/data/DataChunk.h>

namespace urchin {

    class DataParser {
        public:
            explicit DataParser(const std::string&);
            DataParser(const std::string&, const std::string&);

            DataChunk* getRootChunk() const;
            std::vector<DataChunk*> getChunks(const std::string&, const DataAttribute& = DataAttribute(), const DataChunk* parent = nullptr) const;
            DataChunk* getUniqueChunk(bool, const std::string&, const DataAttribute& = DataAttribute(), const DataChunk* parent = nullptr) const;

        private:
            void loadFile(std::ifstream&);
            unsigned int computeIndentLevel(const std::string&) const;
            std::unique_ptr<DataChunk> buildChunk(const std::string& rawContentLine, DataChunk* parent) const;

            std::string getChunkDescription(const std::string&, const DataAttribute&) const;

            static constexpr char NAME_REGEX[] = "([a-zA-Z]+)";
            static constexpr char ATTRIBUTES_REGEX[] = "\\(?(.*?)\\)?";
            static constexpr char VALUE_REGEX[] = "\"?(.*?)\"?";

            std::string filenamePath;
            std::unique_ptr<DataChunk> root;
    };

}
