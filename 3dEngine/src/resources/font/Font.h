#pragma once

#include <UrchinCommon.h>

#include <resources/Resource.h>
#include <resources/image/Image.h>
#include <graphics/api/GraphicsApi.h>

namespace urchin {

    struct Glyph { //glyph is a letter
        unsigned int width;
        unsigned int height;
        int shift;
        std::vector<unsigned char> buf;
    };

    class Font : public Resource {
        public:
            static constexpr unsigned int NUM_LETTERS = 256u; //unicode range 0000-007F (Basic Latin) + 0080-00FF (Latin-1 Supplement)
            static constexpr unsigned int NUM_LETTERS_BY_LINE = 16u;

            Font(unsigned int, const Vector3<float>&, std::shared_ptr<Texture>, std::array<Glyph, NUM_LETTERS>, unsigned int, unsigned int, unsigned int);
            ~Font() override = default;

            unsigned int getFontSize() const;
            Vector3<float> getFontColor() const;

            const Glyph& getGlyph(char32_t) const;
            const std::shared_ptr<Texture>& getTexture() const;
            unsigned int getDimensionTexture() const;
            unsigned int getSpaceBetweenLetters() const;
            unsigned int getSpaceBetweenLines() const;
            unsigned int getHeight() const;

        private:
            unsigned int fontSize;
            Vector3<float> fontColor;

            std::shared_ptr<Texture> alphabetTexture;

            std::array<Glyph, NUM_LETTERS> glyph;
            unsigned int spaceBetweenLetters;
            unsigned int spaceBetweenLines;
            unsigned int height;
    };

}
