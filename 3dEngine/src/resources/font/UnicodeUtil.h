#pragma once

namespace urchin {

    class UnicodeUtil {
        public:
            static constexpr unsigned int NUM_CHARACTERS = 256u; //unicode range 0000-007F (Basic Latin) + 0080-00FF (Latin-1 Supplement)
            static constexpr unsigned int NUM_CHARACTERS_BY_LINE = 16u;

            static bool isCharacterManaged(char32_t);
            static bool isCharacterDisplayable(char32_t);
    };

}
