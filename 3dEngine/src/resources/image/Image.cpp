#include <stdexcept>

#include "resources/image/Image.h"
#include "texture/TextureManager.h"
#include "Image.h"

namespace urchin
{

	Image::Image(unsigned int width, unsigned int height, ImageFormat format, unsigned char *texels8) :
			width(width),
			height(height),
			format(format),
			channelBit(ChannelBit::CHANNEL_8),
			isTexture(false),
			textureID(0)
	{
		this->texels.texels8 = texels8;
	}

	Image::Image(unsigned int width, unsigned int height, ImageFormat format, int16_t *texels16) :
			width(width),
			height(height),
			format(format),
			channelBit(ChannelBit::CHANNEL_16),
			isTexture(false),
			textureID(0)
	{
		this->texels.texels16 = texels16;
	}

	Image::~Image()
	{	
		if(isTexture)
		{
			glDeleteTextures(1, &textureID);
		}else
		{
            if(channelBit==ChannelBit::CHANNEL_8)
            {
                delete[] texels.texels8;
            }else if(channelBit==ChannelBit::CHANNEL_16)
            {
                delete[] texels.texels16;
            }
		}
	}

	unsigned int Image::getWidth() const
	{
		return width;
	}

	unsigned int Image::getHeight() const
	{
		return height;
	}

	Image::ImageFormat Image::getImageFormat() const
	{
		return format;
	}

	Image::ChannelBit Image::getChannelBit() const
	{
		return channelBit;
	}

	unsigned char *Image::getTexels() const
	{
		if(isTexture)
		{
			throw std::runtime_error("The image \"" + getName() + "\" was transformed into a texture, you cannot get the texels.");
		}

		if(channelBit!=ChannelBit::CHANNEL_8)
		{
			throw std::runtime_error("Channel must have 8 bits: " + std::to_string(channelBit));
		}

		return texels.texels8;
	}

	int16_t *Image::getTexels16Bits() const
	{
		if(isTexture)
		{
			throw std::runtime_error("The image \"" + getName() + "\" was transformed into a texture, you cannot get the texels.");
		}

		if(channelBit!=ChannelBit::CHANNEL_16)
		{
			throw std::runtime_error("Channel must have 16 bits: " + std::to_string(channelBit));
		}

		return texels.texels16;
	}

	unsigned int Image::retrieveComponentsCount() const
	{
		if(format==Image::IMAGE_GRAYSCALE)
		{
			return 1;
		}else if(format==Image::IMAGE_RGB)
		{
			return 3;
		}else if(format==Image::IMAGE_RGBA)
		{
			return 4;
		}else
		{
			throw std::runtime_error("Unknown image format: " + std::to_string(format) + ".");
		}
	}

	GLint Image::retrieveInternalFormat() const
	{
		if(format==Image::IMAGE_GRAYSCALE)
		{
			return GL_RED;
		}else if(format==Image::IMAGE_RGB)
		{
			return GL_RGB;
		}else if(format==Image::IMAGE_RGBA)
		{
			return GL_RGBA;
		}else
		{
			throw std::runtime_error("Unknown image format: " + std::to_string(format) + ".");
		}
	}

	GLenum Image::retrieveFormat() const
	{
		if(format==Image::IMAGE_GRAYSCALE)
		{
			return GL_RED;
		}else if(format==Image::IMAGE_RGB)
		{
			return GL_RGB;
		}else if(format==Image::IMAGE_RGBA)
		{
			return GL_RGBA;
		}else
		{
			throw std::runtime_error("Unknown image format: " + std::to_string(format) + ".");
		}
	}

	unsigned int Image::toTexture(bool needMipMaps, bool needAnisotropy, bool needRepeat)
	{
		if(isTexture)
		{
			return textureID;
		}

		if(channelBit!=ChannelBit::CHANNEL_8)
		{
			throw std::runtime_error("Channel bit not supported to convert into texture: " + std::to_string(channelBit));
		}

		glGenTextures(1, &textureID);
		glBindTexture (GL_TEXTURE_2D, textureID);

		if(GLEW_EXT_texture_filter_anisotropic)
		{
			if(needAnisotropy)
			{
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, TextureManager::instance()->getAnisotropy());
			}else
			{
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
			}
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, needMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, needRepeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, needRepeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, retrieveInternalFormat(), width, height, 0, retrieveFormat(), GL_UNSIGNED_BYTE, texels.texels8);

		if(needMipMaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		delete [] texels.texels8; //the API has its own copy
		isTexture=true;

		return textureID;
	}

	unsigned int Image::getTextureID() const
	{
		if(!isTexture)
		{
			throw std::runtime_error("The image \"" + getName() + "\" isn't a texture. Use Image::toTexture() for transform the image into texture.");
		}
		return textureID;
	}
}
