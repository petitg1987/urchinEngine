#include <stdexcept>
#include <cstring>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <UrchinCommon.h>

#include "loader/font/LoaderTTF.h"
#include "resources/image/Image.h"

namespace urchin {

    std::shared_ptr<Font> LoaderTTF::loadFromFile(const std::string& ttfFilename, const std::map<std::string, std::string, std::less<>>& params) {
        assert(params.contains("fontSize"));
        assert(params.contains("fontColor"));
        unsigned int fontSize = TypeConverter::toUnsignedInt(params.find("fontSize")->second);
        Vector3<float> fontColor = TypeConverter::toVector3(params.find("fontColor")->second);

        //initialize freetype
        FT_Library library;
        FT_Face face;
        if (FT_Init_FreeType(&library)) {
            throw std::runtime_error("Error of initialization of FreeType, filename: " + ttfFilename + ".");
        }

        int error = FT_New_Face(library, ttfFilename.c_str(), 0, &face);
        if (error != 0) {
            FT_Done_FreeType(library);
            throw std::runtime_error("The font file is an invalid format or doesn't exist, filename: " + ttfFilename + ", error id: " + std::to_string(error) + ".");
        }

        if (FT_Set_Pixel_Sizes(face, 0, fontSize)) {
            FT_Done_Face(face);
            FT_Done_FreeType(library);

            throw std::runtime_error("Error of specification of the size, filename: " + ttfFilename + ".");
        }

        //filled the struct_glyph
        std::array<Glyph, UnicodeUtil::NUM_CHARACTERS> glyph{};
        FT_UInt glyphIndex = FT_Get_Char_Index(face, 65);
        if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT)) {
            FT_Done_Face(face);
            FT_Done_FreeType(library);
            throw std::runtime_error("Error with the loading of the glyph, filename: " + ttfFilename + ".");
        }
        if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
            FT_Done_Face(face);
            FT_Done_FreeType(library);
            throw std::runtime_error("Error of render with the glyph, filename: " + ttfFilename + ".");
        }
        int bitmapTopA = face->glyph->bitmap_top;

        for (std::size_t i = 0; i < UnicodeUtil::NUM_CHARACTERS;i++) {
            glyphIndex = FT_Get_Char_Index(face, static_cast<FT_ULong>(i));
            if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT)) {
                FT_Done_Face(face);
                FT_Done_FreeType(library);
                throw std::runtime_error("Error with the loading of the glyph, filename: " + ttfFilename + ".");
            }

            if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
                FT_Done_Face(face);
                FT_Done_FreeType(library);
                throw std::runtime_error("Error of render with the glyph, filename: " + ttfFilename + ".");
            }

            glyph[i].shiftX = face->glyph->bitmap_left;
            glyph[i].shiftY = face->glyph->bitmap_top - bitmapTopA;
            glyph[i].letterWidth = (int)(face->glyph->advance.x >> 6);
            glyph[i].letterHeight = (int)(face->glyph->bitmap.rows);
            glyph[i].bitmapWidth = face->glyph->bitmap.width;
            glyph[i].bitmapHeight = face->glyph->bitmap.rows;
            glyph[i].buf.clear();

            if (glyph[i].bitmapWidth > 0 && glyph[i].bitmapWidth > 0) {
                glyph[i].buf.resize(glyph[i].bitmapWidth * glyph[i].bitmapHeight);
                for (unsigned int j = 0; j < (glyph[i].bitmapWidth * glyph[i].bitmapHeight); j++) {
                    glyph[i].buf[j] = face->glyph->bitmap.buffer[j];
                }
            }
        }

        //compute space between lines and height of characters
        unsigned int height = 0;
        for (int i = 'A'; i < 'Z'; i++) {
            height = std::max(height, glyph[(std::size_t)i].bitmapHeight);
        }
        unsigned int spaceBetweenLines = MathFunction::roundToUInt((float)face->size->metrics.height / 64.0f);
        glyph[(int)' '].bitmapWidth = MathFunction::roundToUInt((float)glyph[(int)'A'].bitmapWidth * 0.4f);

        FT_Done_Face(face);
        FT_Done_FreeType(library);

        //size of characters and texture
        unsigned int maxCharactersSize = 0;
        for (unsigned int i = 0; i < UnicodeUtil::NUM_CHARACTERS; ++i) { //seek the largest character
            if (glyph[i].bitmapWidth > maxCharactersSize) {
                maxCharactersSize = glyph[i].bitmapWidth;
            }
            if (glyph[i].bitmapHeight > maxCharactersSize) {
                maxCharactersSize = glyph[i].bitmapHeight;
            }
        }
        unsigned int textureSize = maxCharactersSize * UnicodeUtil::NUM_CHARACTERS_BY_LINE;

        //texture creation
        std::array fontColorRgba = {
            static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255),
            static_cast<unsigned char>(fontColor.Z * 255), static_cast<unsigned char>(0)
        };
        std::vector texels(textureSize * textureSize, fontColorRgba);
        for (unsigned int charOriginY = 0, charIndex = 0; charOriginY < textureSize; charOriginY += maxCharactersSize) {
            for (unsigned int charOriginX = 0; charOriginX < textureSize; charOriginX += maxCharactersSize, charIndex++) {
                const Glyph& currentGlyph = glyph[charIndex];
                for (unsigned int glyphY = 0, glyphBufIndex = 0; glyphY < currentGlyph.bitmapHeight; glyphY++) {
                    std::size_t rowStartIndex = (charOriginY + glyphY) * textureSize;
                    for (unsigned int glyphX = 0; glyphX < currentGlyph.bitmapWidth; glyphX++, glyphBufIndex++) {
                        std::size_t texelIndex = rowStartIndex + (charOriginX + glyphX);
                        texels[texelIndex][3] = currentGlyph.buf[glyphBufIndex];
                    }
                }
            }
        }

        TransparencyData transparencyData = TransparencyData::buildFromAlpha8Bits(0, 255);
        auto alphabetTexture = Texture::build(ttfFilename, textureSize, textureSize, TextureFormat::RGBA_8_UINT_NORM, texels.data(), transparencyData, TextureDataType::INT_8);
        alphabetTexture->enableMipmap(); //TODO apply only for UI 3d

        //clear buffers of characters
        for (std::size_t i = 0; i < UnicodeUtil::NUM_CHARACTERS; i++) {
            glyph[i].buf.clear();
            glyph[i].buf.shrink_to_fit();
        }

        return std::make_shared<Font>(fontSize, fontColor, alphabetTexture, glyph, spaceBetweenLines, height);
    }

}
