#ifndef ENGINE_CIMAGE_H
#define ENGINE_CIMAGE_H

#include "resources/Resource.h"
#include "texture/TextureManager.h"

namespace urchin
{

	class Image : public Resource
	{
		public:
			Image(int, unsigned int, unsigned int, int, unsigned char *);
			~Image();
			
			int getInternalFormat() const;
			unsigned int getWidth() const;
			unsigned int getHeight() const;
			int getFormat() const;
			unsigned char *getTexels() const;
		
			unsigned int toTexture(bool, bool);
			unsigned int getTextureID() const;
			
		private:
			int internalFormat;	//number of component of the image
			unsigned int width;	//width
			unsigned int height; //height
			int format; //RGB, RGBA, LUMINANCE or LUMINANCE_ALPHA
			unsigned char *texels; //picture data
			
			bool isTexture;
			unsigned int textureID;
	};

}

#endif
