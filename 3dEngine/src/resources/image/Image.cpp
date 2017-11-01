#include <stdexcept>
#include <cassert>

#include "resources/image/Image.h"
#include "texture/TextureManager.h"

namespace urchin
{

	Image::Image(unsigned int componentsCount, unsigned int width, unsigned int height, ImageFormat format, unsigned char *texels) :
			componentsCount(componentsCount),
			width(width),
			height(height),
			format(format),
			texels(texels),
			isTexture(false),
			textureID(0)
	{
		#ifdef _DEBUG
			assert(componentsCount >=0 && componentsCount <= 4);
		#endif
	}

	Image::~Image()
	{	
		if(isTexture)
		{
			glDeleteTextures(1, &textureID);
		}else
		{
			delete [] texels;
		}
	}

	unsigned int Image::getComponentsCount() const
	{
		return componentsCount;
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

	unsigned char *Image::getTexels() const
	{
		if(isTexture)
		{
			throw std::runtime_error("The image \"" + getName() + "\" was transformed into a texture, you cannot get the texels.");
		}
		return texels;
	}

	GLint Image::retrieveInternalFormat() const
	{
		if(componentsCount==1)
		{
			return GL_R;
		}else if(componentsCount==2)
		{
			return GL_RG;
		}else if(componentsCount==3)
		{
			return GL_RGB;
		}else if(componentsCount==4)
		{
			return GL_RGBA;
		}else
		{
			throw std::runtime_error("Unknown image components count: " + std::to_string(componentsCount) + ".");
		}
	}

	GLenum Image::retrieveFormat() const
	{
		if(format==Image::IMAGE_LUMINANCE)
		{
			return GL_LUMINANCE;
		}else if(format==Image::IMAGE_LUMINANCE_ALPHA)
		{
			return GL_LUMINANCE_ALPHA;
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
		glTexImage2D(GL_TEXTURE_2D, 0, retrieveInternalFormat(), width, height, 0, retrieveFormat(), GL_UNSIGNED_BYTE, texels);

		if(needMipMaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		delete [] texels; //the API has its own copy
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
