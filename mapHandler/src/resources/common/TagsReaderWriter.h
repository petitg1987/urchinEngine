#pragma once

namespace urchin {

    class TagsReaderWriter {
        public:
            static constexpr char TAGS_SEPARATOR = ',';

            static std::vector<std::string> loadTags(const UdaChunk*, const UdaParser&);
            static void writeTags(UdaChunk&, const std::vector<std::string>&, UdaWriter&);

        private:

    };

}
