#ifndef URCHINENGINE_FONT_H
#define URCHINENGINE_FONT_H

#include "UrchinCommon.h"

#include "resources/Resource.h"
#include "resources/image/Image.h"

namespace urchin
{

    struct Glyph
    { //glyph is a letter
        unsigned int width;
        unsigned int height;
        int shift;
        unsigned char *buf;
    };

    class Font : public Resource
    {
        public:
            Font(Image *, Glyph *, unsigned int, unsigned int, unsigned int);
            ~Font() override;

            const Glyph &getGlyph(unsigned char i) const;
            unsigned int getTextureID() const;
            unsigned int getDimensionTexture() const;
            unsigned int getSpaceBetweenLetters() const;
            unsigned int getSpaceBetweenLines() const;
            unsigned int getHeight() const;

        private:
            Image *texAlphabet;

            Glyph *glyph;
            unsigned int spaceBetweenLetters, spaceBetweenLines, height;
    };

}

#endif
