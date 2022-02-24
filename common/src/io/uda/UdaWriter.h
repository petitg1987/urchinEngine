#pragma once

#include <string>
#include <memory>
#include <fstream>

#include <io/uda/UdaAttribute.h>
#include <io/uda/UdaChunk.h>

namespace urchin {

    class UdaWriter {
        public:
            explicit UdaWriter(std::string);

            UdaChunk& createChunk(std::string, const UdaAttribute& = UdaAttribute(), UdaChunk* = nullptr);

            void saveInFile() const;

        private:
            unsigned int computeIndentLevel(const UdaChunk&) const;
            std::string buildRawContentLine(const UdaChunk&) const;

            std::string filenamePath;
            std::vector<std::unique_ptr<UdaChunk>> rootNodes;
    };

}
