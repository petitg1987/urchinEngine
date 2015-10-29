#include <GL/gl.h>

#include "TextureFilterBuilder.h"
#include "TextureFilter.h"

namespace urchin
{

	TextureFilterBuilder::TextureFilterBuilder() :
		pFilterType(TextureFilter::DOWN_SAMPLE),
		textureWidth(1024),
		textureHeight(1024),
		pTextureType(GL_TEXTURE_2D),
		pTextureNumberLayer(1),
		pTextureInternalFormat(GL_RGB),
		pTextureFormat(GL_RGB)
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

	TextureFilter::FilterType TextureFilterBuilder::getFilterType() const
	{
		return pFilterType;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureSize(unsigned int textureWidth, unsigned int textureHeight)
	{
		this->textureWidth = textureWidth;
		this->textureHeight = textureHeight;
		return this;
	}

	unsigned int TextureFilterBuilder::getTextureWidth() const
	{
		return textureWidth;
	}

	unsigned int TextureFilterBuilder::getTextureHeight() const
	{
		return textureHeight;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureType(unsigned int textureType)
	{
		this->pTextureType = textureType;
		return this;
	}

	unsigned int TextureFilterBuilder::getTextureType() const
	{
		return pTextureType;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureNumberLayer(unsigned int textureNumberLayer)
	{
		this->pTextureNumberLayer = textureNumberLayer;
		return this;
	}

	unsigned int TextureFilterBuilder::getTextureNumberLayer() const
	{
		return pTextureNumberLayer;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureInternalFormat(int textureInternalFormat)
	{
		this->pTextureInternalFormat = textureInternalFormat;
		return this;
	}

	int TextureFilterBuilder::getTextureInternalFormat() const
	{
		return pTextureInternalFormat;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureFormat(unsigned int textureFormat)
	{
		this->pTextureFormat = textureFormat;
		return this;
	}

	unsigned int TextureFilterBuilder::getTextureFormat() const
	{
		return pTextureFormat;
	}

	std::shared_ptr<TextureFilter> TextureFilterBuilder::build()
	{
		return std::make_shared<TextureFilter>(this);
	}

}
