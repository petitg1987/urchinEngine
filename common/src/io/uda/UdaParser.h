#pragma once

#include <string>
#include <memory>
#include <vector>
#include <fstream>

#include <io/uda/UdaAttribute.h>
#include <io/uda/UdaChunk.h>

namespace urchin {

    enum class UdaLoadType {
            LOAD_FILE,
            NO_LOAD
    };

    class UdaParser {
        public:
            UdaParser();
            UdaParser(std::string, UdaLoadType);

            std::vector<UdaChunk*> getChunks(std::string_view = "", const UdaAttribute& = UdaAttribute(), const UdaChunk* parent = nullptr) const;
            UdaChunk* getFirstChunk(bool, std::string_view, const UdaAttribute& = UdaAttribute(), const UdaChunk* parent = nullptr) const;

            UdaChunk& createChunk(std::string, const UdaAttribute& = UdaAttribute(), UdaChunk* = nullptr);
            UdaChunk& addChunk(const UdaChunk&, UdaChunk* = nullptr);
            void removeChunk(UdaChunk&);
            void save() const;

        private:
            void loadFile(std::ifstream&);
            unsigned int computeIndentLevel(std::string_view) const;
            std::unique_ptr<UdaChunk> buildChunk(const std::string& rawContentLine, UdaChunk* parent) const;
            bool isNodeMatchCriteria(const UdaChunk&, std::string_view, const UdaAttribute&) const;
            std::string getChunkDescription(const std::string&, const UdaAttribute&) const;

            unsigned int computeIndentLevel(const UdaChunk&) const;
            std::string buildRawContentLine(const UdaChunk&) const;

            std::string filenamePath;
            std::vector<std::unique_ptr<UdaChunk>> rootNodes;
    };

}
