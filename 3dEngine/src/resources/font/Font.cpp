#include "resources/font/Font.h"

namespace urchin
{
	
	Font::Font(Image *texAlphabet, Glyph *glyph, int spaceBetweenLetters, int spaceBetweenLines, int height) :
		texAlphabet(texAlphabet), glyph(glyph), spaceBetweenLetters(spaceBetweenLetters), spaceBetweenLines(spaceBetweenLines), height(height)
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

	int Font::getSpaceBetweenLetters() const
	{
		return spaceBetweenLetters;
	}

	int Font::getSpaceBetweenLines() const
	{
		return spaceBetweenLines;
	}

	int Font::getHeight() const
	{
		return height;
	}

}
