#pragma once

#include <UrchinCommon.h>

#include <resources/Resource.h>
#include <resources/image/Image.h>

namespace urchin {

    struct Glyph { //glyph is a letter
        unsigned int width;
        unsigned int height;
        int shift;
        unsigned char* buf;
    };

    class Font : public Resource {
        public:
            Font(unsigned int, const Vector3<float>&, std::shared_ptr<Texture>, Glyph*, unsigned int, unsigned int, unsigned int);
            ~Font() override;

            unsigned int getFontSize() const;
            Vector3<float> getFontColor() const;

            const Glyph& getGlyph(unsigned char i) const;
            const std::shared_ptr<Texture>& getTexture() const;
            unsigned int getDimensionTexture() const;
            unsigned int getSpaceBetweenLetters() const;
            unsigned int getSpaceBetweenLines() const;
            unsigned int getHeight() const;

        private:
            unsigned int fontSize;
            Vector3<float> fontColor;

            std::shared_ptr<Texture> alphabetTexture;

            Glyph* glyph;
            unsigned int spaceBetweenLetters, spaceBetweenLines, height;
    };

}
