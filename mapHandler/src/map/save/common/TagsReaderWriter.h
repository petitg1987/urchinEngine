#pragma once

namespace urchin {

    class TagsReaderWriter {
        public:
            static std::vector<std::string> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const std::vector<std::string>&, UdaWriter&);

        private:
            static constexpr char TAGS_SEPARATOR = ',';
    };

}
