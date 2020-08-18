#include "resources/font/Font.h"

namespace urchin
{

    Font::Font(Image *texAlphabet, Glyph *glyph, unsigned int spaceBetweenLetters, unsigned int spaceBetweenLines, unsigned int height) :
        texAlphabet(texAlphabet),
        glyph(glyph),
        spaceBetweenLetters(spaceBetweenLetters),
        spaceBetweenLines(spaceBetweenLines),
        height(height)
    {

    }

    Font::~Font()
    {
        delete texAlphabet;
        delete [] glyph;
    }

    const Glyph &Font::getGlyph(unsigned char character) const
    {
        return glyph[(unsigned int)character];
    }

    unsigned int Font::getTextureID() const
    {
        return texAlphabet->getTextureID();
    }

    unsigned int Font::getDimensionTexture() const
    {
        return texAlphabet->getWidth();
    }

    unsigned int Font::getSpaceBetweenLetters() const
    {
        return spaceBetweenLetters;
    }

    unsigned int Font::getSpaceBetweenLines() const
    {
        return spaceBetweenLines;
    }

    unsigned int Font::getHeight() const
    {
        return height;
    }

}
