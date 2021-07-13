#pragma once

#include <string>
#include <memory>
#include <fstream>

#include <io/uda/UdaAttribute.h>
#include <io/uda/UdaChunk.h>

namespace urchin {

    class UdaWriter {
        public:
            explicit UdaWriter(const std::string&);

            UdaChunk& createChunk(const std::string&, const UdaAttribute& = UdaAttribute(), UdaChunk* = nullptr);
            void saveInFile();

        private:
            unsigned int computeIndentLevel(const UdaChunk&) const;
            std::string buildRawContentLine(const UdaChunk&) const;

            std::string filenamePath;
            std::unique_ptr<UdaChunk> root;
    };

}
