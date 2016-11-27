#ifndef ENGINE_CIMAGE_H
#define ENGINE_CIMAGE_H

#include "resources/Resource.h"

namespace urchin
{

	class Image : public Resource
	{
		public:
			enum ImageFormat
			{
				IMAGE_RGB,
				IMAGE_RGBA,
				IMAGE_LUMINANCE,
				IMAGE_LUMINANCE_ALPHA
			};

			Image(unsigned int, unsigned int, unsigned int, ImageFormat, unsigned char *);
			~Image();

			unsigned int getComponentsCount() const;
			unsigned int getWidth() const;
			unsigned int getHeight() const;
			ImageFormat getImageFormat() const;
			unsigned char *getTexels() const;

			int retrieveInternalFormat() const;
			int retrieveFormat() const;
		
			unsigned int toTexture(bool, bool);
			unsigned int getTextureID() const;
			
		private:
			unsigned int componentsCount;
			unsigned int width;
			unsigned int height;
			ImageFormat format;
			unsigned char *texels;
			
			bool isTexture;
			unsigned int textureID;
	};

}

#endif
