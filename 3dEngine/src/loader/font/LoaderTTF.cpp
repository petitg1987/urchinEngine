#include <stdexcept>
#include <cstring>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <UrchinCommon.h>

#include <loader/font/LoaderTTF.h>
#include <resources/image/Image.h>

namespace urchin {

    std::shared_ptr<Font> LoaderTTF::loadFromFile(const std::string& ttfFilename, const std::map<std::string, std::string, std::less<>>& params) {
        assert(params.find("fontSize") != params.end());
        assert(params.find("fontColor") != params.end());
        unsigned int fontSize = TypeConverter::toUnsignedInt(params.find("fontSize")->second);
        Vector3<float> fontColor = TypeConverter::toVector3(params.find("fontColor")->second);
        std::array<unsigned char, 3> fontColor255 = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255)};

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

            glyph[i].shift = face->glyph->bitmap_top - bitmapTopA;
            glyph[i].width = face->glyph->bitmap.width;
            glyph[i].height = face->glyph->bitmap.rows;
            glyph[i].buf.clear();

            if (glyph[i].width > 0 && glyph[i].height > 0) {
                glyph[i].buf.resize(glyph[i].width * glyph[i].height);
                for (unsigned int j = 0; j < (glyph[i].width * glyph[i].height); j++) {
                    glyph[i].buf[j] = face->glyph->bitmap.buffer[j];
                }
            }
        }

        FT_Done_Face(face);
        FT_Done_FreeType(library);

        //compute space between lines, space between characters and height of characters
        unsigned int height = 0;
        for (int i = 'A'; i < 'Z'; i++) {
            height = std::max(height, glyph[(std::size_t)i].height);
        }
        unsigned int spaceBetweenLines = MathFunction::roundToUInt((float)height * 1.9f);
        unsigned int spaceBetweenCharacters = 2u;
        glyph[(int)' '].width = MathFunction::roundToUInt((float)glyph[(int)'A'].width * 0.4f);

        //dimension of characters and texture
        unsigned int dimensionCharacters = 0;
        for (unsigned int i = 0; i < UnicodeUtil::NUM_CHARACTERS; ++i) { //seek the largest character
            if (glyph[i].width > dimensionCharacters) {
                dimensionCharacters = glyph[i].width;
            }
            if (glyph[i].height > dimensionCharacters) {
                dimensionCharacters = glyph[i].height;
            }
        }
        unsigned int dimensionTexture = dimensionCharacters * UnicodeUtil::NUM_CHARACTERS_BY_LINE;

        //texture creation
        constexpr unsigned int NUM_COLORS = 4u;
        std::vector<unsigned char> texels(dimensionTexture * dimensionTexture * NUM_COLORS, 0);
        for (unsigned int i = 0, c = 0; i < dimensionTexture; i += dimensionCharacters) {
            for (unsigned int j = 0; j < dimensionTexture; j += dimensionCharacters, c++) {

                const Glyph& currentGlyph = glyph[c];
                for (unsigned int yy = 0, m = 0; yy < currentGlyph.height; yy++) {
                    std::size_t baseYIndex = (i + yy) * dimensionTexture * NUM_COLORS;
                    for (unsigned int xx = 0; xx < currentGlyph.width; xx++, m++) {
                        std::size_t baseIndex = baseYIndex + ((j + xx) * NUM_COLORS);

                        if (currentGlyph.buf[m] > 0) {
                            std::memcpy(&texels[baseIndex], fontColor255.data(), 3 * sizeof(unsigned char));
                        }
                        texels[baseIndex + 3] = currentGlyph.buf[m];
                    }
                }
            }
        }

        auto alphabetTexture = Texture::build(ttfFilename, dimensionTexture, dimensionTexture, TextureFormat::RGBA_8_INT, texels.data(), TextureDataType::INT_8);

        //clear buffers of characters
        for (std::size_t i = 0; i < UnicodeUtil::NUM_CHARACTERS; i++) {
            glyph[i].buf.clear();
            glyph[i].buf.shrink_to_fit();
        }

        return std::make_shared<Font>(fontSize, fontColor, alphabetTexture, glyph, spaceBetweenCharacters, spaceBetweenLines, height);
    }

}
