#include <resources/font/Font.h>

namespace urchin {

    Font::Font(unsigned int fontSize, const Vector3<float>& fontColor, std::shared_ptr<Texture> alphabetTexture, Glyph* glyph,
               unsigned int spaceBetweenLetters, unsigned int spaceBetweenLines, unsigned int height) :
        fontSize(fontSize),
        fontColor(fontColor),
        alphabetTexture(std::move(alphabetTexture)),
        glyph(glyph),
        spaceBetweenLetters(spaceBetweenLetters),
        spaceBetweenLines(spaceBetweenLines),
        height(height) {

    }

    Font::~Font() {
        delete[] glyph;
    }

    unsigned int Font::getFontSize() const {
        return fontSize;
    }

    Vector3<float> Font::getFontColor() const {
        return fontColor;
    }

    const Glyph& Font::getGlyph(char32_t unicodeCharacter) const {
        if(unicodeCharacter > 0xFF) {
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

    unsigned int Font::getSpaceBetweenLetters() const {
        return spaceBetweenLetters;
    }

    unsigned int Font::getSpaceBetweenLines() const {
        return spaceBetweenLines;
    }

    unsigned int Font::getHeight() const {
        return height;
    }

}
