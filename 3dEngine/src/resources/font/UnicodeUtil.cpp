#include <resources/font/UnicodeUtil.h>

namespace urchin {

    bool UnicodeUtil::isCharacterManaged(char32_t unicodeCharacter) {
        return unicodeCharacter <= 0xFF;
    }

    bool UnicodeUtil::isCharacterDisplayable(char32_t unicodeCharacter) {
        return isCharacterManaged(unicodeCharacter)
                && unicodeCharacter != 8 //ignore 'Backspace' unicode
               && unicodeCharacter != 127 //ignore 'Delete' unicode
               && unicodeCharacter > 0x1F //ignore 'Controls C0' unicode
               && (unicodeCharacter < 0x80 || unicodeCharacter > 0x9F); //ignore 'Controls C1' unicode

    }

}