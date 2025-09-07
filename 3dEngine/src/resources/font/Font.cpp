#include "resources/font/Font.h"

#include <utility>

namespace urchin {

    Font::Font(unsigned int fontSize, const Vector3<float>& fontColor, std::shared_ptr<Texture> alphabetTexture, std::array<Glyph, UnicodeUtil::NUM_CHARACTERS> glyph,
               unsigned int spaceBetweenLines, unsigned int height) :
            fontSize(fontSize),
            fontColor(fontColor),
            alphabetTexture(std::move(alphabetTexture)),
            glyph(std::move(glyph)),
            spaceBetweenLines(spaceBetweenLines),
            height(height) {

    }

    unsigned int Font::getFontSize() const {
        return fontSize;
    }

    Vector3<float> Font::getFontColor() const {
        return fontColor;
    }

    const Glyph& Font::getGlyph(char32_t unicodeCharacter) const {
        if (!UnicodeUtil::isCharacterManaged(unicodeCharacter)) {
            throw std::out_of_range("Unicode character " + std::to_string(unicodeCharacter) + " cannot be retrieved (not implemented)");
        }
        return glyph[unicodeCharacter];
    }

    const std::shared_ptr<Texture>& Font::getTexture() const {
        return alphabetTexture;
    }

    unsigned int Font::getDimensionTexture() const {
        return alphabetTexture->getWidth();
    }

    unsigned int Font::getSpaceBetweenLines() const {
        return spaceBetweenLines;
    }

    unsigned int Font::getHeight() const {
        return height;
    }

}
