#ifndef ENGINE_FONT_H
#define ENGINE_FONT_H

#include "resources/Resource.h"
#include "resources/image/Image.h"

namespace urchin
{
	
	struct Glyph
	{ //glyph is a letter
		int width;
		int height;
		int shift;
		unsigned char *buf;
	};

	class Font : public Resource
	{
		public:
			Font(Image *, Glyph *, int, int, int);
			~Font();
		
			const Glyph &getGlyph(unsigned char i) const;
			unsigned int getTextureID() const;
			unsigned int getDimensionTexture() const;
			int getSpaceBetweenLetters() const;
			int getSpaceBetweenLines() const;
			int getHeight() const;
		
		private:
			Image *texAlphabet;
		
			Glyph *glyph;
			int spaceBetweenLetters, spaceBetweenLines, height;
	};

}

#endif
