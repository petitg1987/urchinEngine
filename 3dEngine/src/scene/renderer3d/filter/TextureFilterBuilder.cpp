#include <GL/gl.h>

#include "TextureFilterBuilder.h"

namespace urchin
{

	TextureFilterBuilder::TextureFilterBuilder() :
		pFilterType(TextureFilter::DOWN_SAMPLE),
		textureWidth(1024),
		textureHeight(1024),
		pTextureType(GL_TEXTURE_2D),
		pTextureNumberLayer(1),
		pTextureInternalFormat(GL_RGBA),
		pTextureFormat(GL_RGBA)
	{

	}

	TextureFilterBuilder::~TextureFilterBuilder()
	{

	}

	TextureFilterBuilder *TextureFilterBuilder::filterType(TextureFilter::FilterType filterType)
	{
		this->pFilterType = filterType;
		return this;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureSize(unsigned int textureWidth, unsigned int textureHeight)
	{
		this->textureWidth = textureWidth;
		this->textureHeight = textureHeight;
		return this;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureType(unsigned int textureType)
	{
		this->pTextureType = textureType;
		return this;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureNumberLayer(unsigned int textureNumberLayer)
	{
		this->pTextureNumberLayer = textureNumberLayer;
		return this;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureInternalFormat(int textureInternalFormat)
	{
		this->pTextureInternalFormat = textureInternalFormat;
		return this;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureFormat(unsigned int textureFormat)
	{
		this->pTextureFormat = textureFormat;
		return this;
	}

	TextureFilter *TextureFilterBuilder::build()
	{
		return new TextureFilter(this); //TODO delete this new object..
	}

}
