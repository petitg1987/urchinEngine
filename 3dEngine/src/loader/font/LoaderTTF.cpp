#include <stdexcept>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "UrchinCommon.h"

#include "loader/font/LoaderTTF.h"
#include "resources/image/Image.h"

#define WIDTH_BETWEEN_LETTERS 2
#define WIDTH_BETWEEN_LINES_RATE 1.9f
#define WIDTH_SPACE_RATE 0.4f
#define NUM_COLORS 4
#define NUM_LETTERS 256
#define NUM_LETTERS_BY_LINE 16

namespace urchin
{

	Font *LoaderTTF::loadFromFile(const std::string &fileFont, void *params)
	{
		//parameters
		FontParameters textParameters = (*static_cast<FontParameters*>(params));
		
		//initialize freetype
		std::string fileFontPath = FileSystem::instance()->getResourcesDirectory() + fileFont;
		FT_Library library;
		FT_Face face;
		if (FT_Init_FreeType(&library))
		{
			throw std::runtime_error("Error of initialization of FreeType, filename: " + fileFontPath + ".");
		}
		
		int error = FT_New_Face(library, fileFontPath.c_str(), 0, &face);
		if(error!=0)
		{
			FT_Done_FreeType(library);
			throw std::runtime_error("The font file is an invalid format or doesn't exist, filename: " + fileFontPath + ", error id: " + std::to_string(error) + ".");
		}

		if (FT_Set_Char_Size(face, 0, textParameters.fontSize << 6, 96, 96))
		{
			FT_Done_Face(face);
			FT_Done_FreeType(library);

			throw std::runtime_error("Error of specification of the size, filename: " + fileFontPath + ".");
		}
		
		//filled the struct_glyph
		auto *glyph = new Glyph[NUM_LETTERS];

		FT_UInt glyphIndex = FT_Get_Char_Index(face, 65);
		if(FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT))
		{
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			delete [] glyph;

			throw std::runtime_error("Error with the loading of the glyph, filename: " + fileFontPath + ".");
		}
		if(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
		{
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			delete [] glyph;

			throw std::runtime_error("Error of render with the glyph, filename: " + fileFontPath + ".");
		}
		int bitmapTopA = face->glyph->bitmap_top;
		
		for(int i=0;i<NUM_LETTERS;i++)
		{
			glyphIndex = FT_Get_Char_Index(face, static_cast<FT_ULong>(i));
			if(FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT))
			{
				FT_Done_Face(face);
				FT_Done_FreeType(library);
				delete [] glyph;

				throw std::runtime_error("Error with the loading of the glyph, filename: " + fileFontPath + ".");
			}
		
			if(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
			{
				FT_Done_Face(face);
				FT_Done_FreeType(library);
				delete [] glyph;

				throw std::runtime_error("Error of render with the glyph, filename: " + fileFontPath + ".");
			}
			
			glyph[i].shift = face->glyph->bitmap_top - bitmapTopA;
			glyph[i].width = face->glyph->bitmap.width;
			glyph[i].height = face->glyph->bitmap.rows;
			glyph[i].buf = nullptr;

			if(glyph[i].width > 0 && glyph[i].height > 0)
			{
				glyph[i].buf = new unsigned char[glyph[i].width * glyph[i].height];
				for(int j=0;j<glyph[i].width * glyph[i].height;j++)
				{
					glyph[i].buf[j] = face->glyph->bitmap.buffer[j];
				}
			}
		}
		
		FT_Done_Face(face);
		FT_Done_FreeType(library);

		//compute space between lines, space between letters and height of letters
		unsigned int height = 0;
		for(int i='A'; i<'Z';i++)
		{
			height = std::max(height, (unsigned int)glyph[i].height);
		}
		auto spaceBetweenLines = static_cast<unsigned int>(height * WIDTH_BETWEEN_LINES_RATE);
		auto spaceBetweenLetters = static_cast<unsigned int>(WIDTH_BETWEEN_LETTERS);
		glyph[' '].width = (int)((float)glyph['A'].width * WIDTH_SPACE_RATE);

		//dimension of letters and texture
		int dimensionLetters=0;
		for(int i=0;i<NUM_LETTERS;++i) //seek the largest letter
		{
			if(glyph[i].width > dimensionLetters)
			{
				dimensionLetters = glyph[i].width;
			}
			if(glyph[i].height > dimensionLetters)
			{
				dimensionLetters = glyph[i].height;
			}
		}
		auto dimensionTexture = static_cast<unsigned int>(dimensionLetters * NUM_LETTERS_BY_LINE);
		
		//texture creation
		std::vector<unsigned char> texels(dimensionTexture*dimensionTexture*NUM_COLORS, 0);
		Vector3<float> fontColor = textParameters.fontColor;
		for(unsigned int i=0,c=0; i<dimensionTexture; i+=dimensionLetters)
		{
			for(unsigned int j=0; j<dimensionTexture; j+=dimensionLetters,c++)
			{
				for(int yy=0,m=0; yy<glyph[c].height; yy++)
				{
					for(int xx=0; xx<glyph[c].width; xx++,m++)
					{
						texels[ ((i+yy)*dimensionTexture*NUM_COLORS) + ((j+xx)*NUM_COLORS) + 0] = (glyph[c].buf[m]>0) ? static_cast<int>(fontColor.X*255) : 0;
						texels[ ((i+yy)*dimensionTexture*NUM_COLORS) + ((j+xx)*NUM_COLORS) + 1] = (glyph[c].buf[m]>0) ? static_cast<int>(fontColor.Y*255) : 0;
						texels[ ((i+yy)*dimensionTexture*NUM_COLORS) + ((j+xx)*NUM_COLORS) + 2] = (glyph[c].buf[m]>0) ? static_cast<int>(fontColor.Z*255) : 0;

						texels[ ((i+yy)*dimensionTexture*NUM_COLORS) + ((j+xx)*NUM_COLORS) + 3] = glyph[c].buf[m];
					}
				}
			}
		}

		auto *texAlphabet = new Image(dimensionTexture, dimensionTexture, Image::IMAGE_RGBA, std::move(texels));
		texAlphabet->toTexture(false, false, false);
		
		//clears buffers of letters
		for(int i=0;i<NUM_LETTERS;i++)
		{
			delete [] glyph[i].buf;
			glyph[i].buf = nullptr;
		}

		return (new Font(texAlphabet, glyph, spaceBetweenLetters, spaceBetweenLines, height));
	}

}
