#include <stdexcept>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <UrchinCommon.h>

#include <loader/font/LoaderTTF.h>
#include <resources/image/Image.h>

namespace urchin {

    std::shared_ptr<Font> LoaderTTF::loadFromFile(const std::string& ttfFilename, const std::map<std::string, std::string>& params) {
        std::locale::global(std::locale("C")); //for float

        assert(params.find("fontSize") != params.end());
        assert(params.find("fontColor") != params.end());
        unsigned int fontSize = TypeConverter::toUnsignedInt(params.find("fontSize")->second);
        Vector3<float> fontColor = TypeConverter::toVector3(params.find("fontColor")->second);

        //initialize freetype
        std::string fileFontPath = FileSystem::instance()->getResourcesDirectory() + ttfFilename;
        FT_Library library;
        FT_Face face;
        if (FT_Init_FreeType(&library)) {
            throw std::runtime_error("Error of initialization of FreeType, filename: " + fileFontPath + ".");
        }

        int error = FT_New_Face(library, fileFontPath.c_str(), 0, &face);
        if (error != 0) {
            FT_Done_FreeType(library);
            throw std::runtime_error("The font file is an invalid format or doesn't exist, filename: " + fileFontPath + ", error id: " + std::to_string(error) + ".");
        }

        if (FT_Set_Pixel_Sizes(face, 0, fontSize)) {
            FT_Done_Face(face);
            FT_Done_FreeType(library);

            throw std::runtime_error("Error of specification of the size, filename: " + fileFontPath + ".");
        }

        //filled the struct_glyph
        std::array<Glyph, Font::NUM_LETTERS> glyph{};
        FT_UInt glyphIndex = FT_Get_Char_Index(face, 65);
        if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT)) {
            FT_Done_Face(face);
            FT_Done_FreeType(library);
            throw std::runtime_error("Error with the loading of the glyph, filename: " + fileFontPath + ".");
        }
        if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
            FT_Done_Face(face);
            FT_Done_FreeType(library);
            throw std::runtime_error("Error of render with the glyph, filename: " + fileFontPath + ".");
        }
        int bitmapTopA = face->glyph->bitmap_top;

        for (std::size_t i = 0; i < Font::NUM_LETTERS;i++) {
            glyphIndex = FT_Get_Char_Index(face, static_cast<FT_ULong>(i));
            if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT)) {
                FT_Done_Face(face);
                FT_Done_FreeType(library);
                throw std::runtime_error("Error with the loading of the glyph, filename: " + fileFontPath + ".");
            }

            if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
                FT_Done_Face(face);
                FT_Done_FreeType(library);
                throw std::runtime_error("Error of render with the glyph, filename: " + fileFontPath + ".");
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

        //compute space between lines, space between letters and height of letters
        unsigned int height = 0;
        for (int i = 'A'; i < 'Z'; i++) {
            height = std::max(height, glyph[(std::size_t)i].height);
        }
        auto spaceBetweenLines = (unsigned int)((float)height * 1.9f);
        auto spaceBetweenLetters = (unsigned int)2u;
        glyph[(int)' '].width = (unsigned int)((float)glyph[(int)'A'].width * 0.4f);

        //dimension of letters and texture
        unsigned int dimensionLetters = 0;
        for (unsigned int i = 0; i < Font::NUM_LETTERS; ++i) { //seek the largest letter
            if (glyph[i].width > dimensionLetters) {
                dimensionLetters = glyph[i].width;
            }
            if (glyph[i].height > dimensionLetters) {
                dimensionLetters = glyph[i].height;
            }
        }
        unsigned int dimensionTexture = dimensionLetters * Font::NUM_LETTERS_BY_LINE;

        //texture creation
        constexpr unsigned int NUM_COLORS = 4u;
        std::vector<unsigned char> texels(dimensionTexture * dimensionTexture * NUM_COLORS, 0);
        for (unsigned int i = 0, c = 0; i < dimensionTexture; i += dimensionLetters) {
            for (unsigned int j = 0; j < dimensionTexture; j += dimensionLetters, c++) {
                for (unsigned int yy = 0, m = 0; yy < glyph[c].height; yy++) {
                    for (unsigned int xx = 0; xx < glyph[c].width; xx++,m++) {
                        texels[ ((i + yy) * dimensionTexture * NUM_COLORS) + ((j + xx) * NUM_COLORS) + 0] = (glyph[c].buf[m] > 0) ? static_cast<unsigned char>(fontColor.X * 255) : 0;
                        texels[ ((i + yy) * dimensionTexture * NUM_COLORS) + ((j + xx) * NUM_COLORS) + 1] = (glyph[c].buf[m] > 0) ? static_cast<unsigned char>(fontColor.Y * 255) : 0;
                        texels[ ((i + yy) * dimensionTexture * NUM_COLORS) + ((j + xx) * NUM_COLORS) + 2] = (glyph[c].buf[m] > 0) ? static_cast<unsigned char>(fontColor.Z * 255) : 0;

                        texels[ ((i + yy) * dimensionTexture * NUM_COLORS) + ((j + xx) * NUM_COLORS) + 3] = glyph[c].buf[m];
                    }
                }
            }
        }

        auto alphabetTexture = Image(dimensionTexture, dimensionTexture, Image::IMAGE_RGBA, std::move(texels)).createTexture(false);

        //clears buffers of letters
        for (std::size_t i = 0; i < Font::NUM_LETTERS; i++) {
            glyph[i].buf.clear();
        }

        return std::make_shared<Font>(fontSize, fontColor, alphabetTexture, glyph, spaceBetweenLetters, spaceBetweenLines, height);
    }

}
