#pragma once

#include <UrchinCommon.h>

#include "resources/Resource.h"
#include "resources/font/UnicodeUtil.h"
#include "graphics/api/GraphicsApi.h"

namespace urchin {

    struct Glyph { //glyph is a letter
        unsigned int bitmapWidth;
        unsigned int bitmapHeight;
        int shiftX;
        int shiftY;
        int letterWidth;
        std::vector<unsigned char> buf;
    };

    class Font final : public Resource {
        public:
            Font(unsigned int, const Vector3<float>&, std::shared_ptr<Texture>, std::array<Glyph, UnicodeUtil::NUM_CHARACTERS>, unsigned int, unsigned int, unsigned int);
            ~Font() override = default;

            unsigned int getFontSize() const;
            Vector3<float> getFontColor() const;

            const Glyph& getGlyph(char32_t) const;
            const std::shared_ptr<Texture>& getTexture() const;
            unsigned int getDimensionTexture() const;
            unsigned int getSpaceBetweenCharacters() const;
            unsigned int getSpaceBetweenLines() const;
            unsigned int getHeight() const;

        private:
            unsigned int fontSize;
            Vector3<float> fontColor;

            std::shared_ptr<Texture> alphabetTexture;

            std::array<Glyph, UnicodeUtil::NUM_CHARACTERS> glyph;
            unsigned int spaceBetweenCharacters;
            unsigned int spaceBetweenLines;
            unsigned int height;
    };

}
