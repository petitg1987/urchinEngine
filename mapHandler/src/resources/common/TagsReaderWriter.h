#pragma once

namespace urchin {

    class TagsReaderWriter {
        public:
            static constexpr char TAGS_SEPARATOR = ',';

            static std::vector<std::string> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const std::vector<std::string>&, UdaWriter&);

        private:

    };

}
