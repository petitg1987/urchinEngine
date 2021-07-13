#pragma once

#include <string>
#include <memory>
#include <fstream>

#include <io/uda/UdaAttribute.h>
#include <io/uda/DataChunk.h>

namespace urchin {

    class UdaWriter {
        public:
            explicit UdaWriter(const std::string&);

            DataChunk& createChunk(const std::string&, const UdaAttribute& = UdaAttribute(), DataChunk* = nullptr);
            void saveInFile();

        private:
            unsigned int computeIndentLevel(const DataChunk&) const;
            std::string buildRawContentLine(const DataChunk&) const;

            std::string filenamePath;
            std::unique_ptr<DataChunk> root;
    };

}
