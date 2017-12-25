#ifndef URCHINENGINE_CIMAGE_H
#define URCHINENGINE_CIMAGE_H

#include <GL/glew.h>

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
				IMAGE_GRAYSCALE
			};
			enum ChannelBit
			{
				CHANNEL_8,
				CHANNEL_16
			};

			Image(unsigned int, unsigned int, ImageFormat, unsigned char *);
			Image(unsigned int, unsigned int, ImageFormat, int16_t *);
			~Image() override;

			unsigned int getWidth() const;
			unsigned int getHeight() const;
			ImageFormat getImageFormat() const;
			ChannelBit getChannelBit() const;
			unsigned char *getTexels() const;
			int16_t *getTexels16Bits() const;

			unsigned int retrieveComponentsCount() const;
			GLint retrieveInternalFormat() const;
			GLenum retrieveFormat() const;
		
			unsigned int toTexture(bool, bool, bool);
			unsigned int getTextureID() const;
			
		private:
			unsigned int width;
			unsigned int height;
			ImageFormat format;
			ChannelBit channelBit;
			union
			{
				unsigned char *texels8; //8 bits
				int16_t *texels16; //16 bits
			} texels;
			
			bool isTexture;
			unsigned int textureID;
	};

}

#endif
