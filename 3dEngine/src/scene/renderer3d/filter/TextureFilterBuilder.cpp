#include <GL/gl.h>
#include <stdexcept>

#include "TextureFilterBuilder.h"
#include "TextureFilter.h"
#include "DownSampleFilter.h"
#include "GaussianBlurFilter.h"

namespace urchin
{

	TextureFilterBuilder::TextureFilterBuilder() :
		pFilterType(DOWN_SAMPLE),
		textureWidth(1024),
		textureHeight(1024),
		pTextureType(GL_TEXTURE_2D),
		pTextureAccessFilter(GL_LINEAR),
		pTextureAnisotropy(1.0f),
		pTextureNumberLayer(1),
		pTextureInternalFormat(GL_RGB),
		pTextureFormat(GL_RGB),
		pBlurSize(3) //3x3 blur
	{

	}

	TextureFilterBuilder::~TextureFilterBuilder()
	{

	}

	TextureFilterBuilder *TextureFilterBuilder::filterType(FilterType filterType)
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

	TextureFilterBuilder *TextureFilterBuilder::textureAccessFilter(unsigned int)
	{
		this->pTextureAccessFilter = pTextureAccessFilter;
		return this;
	}

	unsigned int TextureFilterBuilder::getTextureAccessFilter() const
	{
		return pTextureAccessFilter;
	}

	TextureFilterBuilder *TextureFilterBuilder::textureAnisotropy(float pTextureAnisotropy)
	{
		this->pTextureAnisotropy = pTextureAnisotropy;
		return this;
	}

	float TextureFilterBuilder::getTextureAnisotropy() const
	{
		return pTextureAnisotropy;
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

	TextureFilterBuilder *TextureFilterBuilder::blurSize(unsigned int blurSize)
	{
		this->pBlurSize = blurSize;
		return this;
	}

	unsigned int TextureFilterBuilder::getBlurSize() const
	{
		return pBlurSize;
	}

	std::shared_ptr<TextureFilter> TextureFilterBuilder::build()
	{
		std::shared_ptr<TextureFilter> textureFilter;

		if(pFilterType==DOWN_SAMPLE)
		{
			textureFilter = std::make_shared<DownSampleFilter>(this);
		}else if(pFilterType==GAUSSIAN_BLUR_V)
		{
			textureFilter = std::make_shared<GaussianBlurFilter>(this, GaussianBlurFilter::VERTICAL);
		}else if(pFilterType==GAUSSIAN_BLUR_H)
		{
			textureFilter = std::make_shared<GaussianBlurFilter>(this, GaussianBlurFilter::HORIZONTAL);
		}else
		{
			throw std::invalid_argument("Unknown filter type: " + pFilterType);
		}

		textureFilter->initialize();

		return textureFilter;
	}

}
