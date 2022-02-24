#pragma once

#include <string>
#include <memory>
#include <vector>
#include <fstream>

#include <io/uda/UdaAttribute.h>
#include <io/uda/UdaChunk.h>

namespace urchin {

    class UdaParser {
        public:
            explicit UdaParser(std::string);

            std::vector<UdaChunk*> getChunks(std::string_view = "", const UdaAttribute& = UdaAttribute(), const UdaChunk* parent = nullptr) const;
            UdaChunk* getFirstChunk(bool, std::string_view, const UdaAttribute& = UdaAttribute(), const UdaChunk* parent = nullptr) const;

        private:
            void loadFile(std::ifstream&);
            unsigned int computeIndentLevel(std::string_view) const;
            std::unique_ptr<UdaChunk> buildChunk(const std::string& rawContentLine, UdaChunk* parent) const;

            bool isNodeMatchCriteria(const UdaChunk&, std::string_view, const UdaAttribute&) const;
            std::string getChunkDescription(const std::string&, const UdaAttribute&) const;

            static constexpr char NAME_REGEX[] = "([a-zA-Z]+)";
            static constexpr char ATTRIBUTES_REGEX[] = "\\(?(.*?)\\)?";
            static constexpr char VALUE_REGEX[] = "\"?(.*?)\"?";

            std::string filenamePath;
            std::vector<std::unique_ptr<UdaChunk>> rootNodes;
    };

}
