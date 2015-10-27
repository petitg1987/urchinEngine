#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdexcept>
#include "UrchinCommon.h"

#include "resources/image/Image.h"

namespace urchin
{

	Image::Image(int internalFormat, unsigned int width, unsigned int height, int format, unsigned char *texels) : Resource(),
			internalFormat(internalFormat),
			width(width),
			height(height),
			format(format),
			texels(texels),
			isTexture(false),
			textureID(0),
			maxCoordS(0),
			maxCoordT(0)
	{

	}

	Image::~Image()
	{	
		if(isTexture==true)
		{
			glDeleteTextures(1, &textureID);
		}else
		{
			delete [] texels;
		}
	}

	int Image::getInternalFormat() const
	{
		return internalFormat;
	}

	unsigned int Image::getWidth() const
	{
		return width;
	}

	unsigned int Image::getHeight() const
	{
		return height;
	}

	int Image::getFormat() const
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

	unsigned int Image::toTexture(bool needMipmaps, bool needAnisotropy)
	{
		if(isTexture)
		{
			return textureID;
		}

		//find the next power 2 of width/height
		unsigned int newWidth = MathAlgorithm::nextPowerOfTwo(width);
		unsigned int newHeight = MathAlgorithm::nextPowerOfTwo(height);

		if(newWidth!=width || newHeight!=height) //reScale image
		{
			unsigned char *newTexels = new unsigned char[newWidth*newHeight*internalFormat];
			//copy texInfo->texels to newTexels with reScale :
			for(unsigned int i=0;i<height;i++)
			{
				for(unsigned int j=0;j<width*internalFormat;j++)
				{
					newTexels[j+i*newWidth*internalFormat] = texels[j+i*width*internalFormat];
				}
			}
				
			maxCoordS = (float)width/(float)newWidth;
			maxCoordT = (float)height/(float)newHeight;
			
			delete [] texels;
			texels = newTexels;
		}else
		{
			maxCoordS = 1.0;
			maxCoordT = 1.0;
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, needMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, newWidth, newHeight, 0, format, GL_UNSIGNED_BYTE, texels);

		if(needMipmaps)
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

	float Image::getMaxCoordS() const
	{
		return maxCoordS;
	}

	float Image::getMaxCoordT() const
	{
		return maxCoordT;
	}

}
