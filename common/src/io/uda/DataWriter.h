#pragma once

#include <string>
#include <memory>
#include <fstream>

#include <io/uda/DataAttribute.h>
#include <io/uda/DataChunk.h>

namespace urchin {

    class DataWriter {
        public:
            explicit DataWriter(const std::string&);

            DataChunk& createChunk(const std::string&, const DataAttribute & = DataAttribute(), DataChunk* parent = nullptr);
            void saveInFile();

        private:
            unsigned int computeIndentLevel(const DataChunk&) const;
            std::string buildRawContentLine(const DataChunk&) const;

            std::string filenamePath;
            std::unique_ptr<DataChunk> root;
    };

}
