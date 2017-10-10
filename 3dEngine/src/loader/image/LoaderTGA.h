#ifndef URCHINENGINE_LOADERTGA_H
#define URCHINENGINE_LOADERTGA_H

#include <string>

#include "resources/image/Image.h"
#include "loader/Loader.h"

namespace urchin
{
	
	#pragma pack(push, 1)
	struct TgaHeader
	{
		unsigned char idLenght; //size of image id
		unsigned char colormapType; //1 is has a color map
		unsigned char imageType; //compression type

		short cmFirstEntry; //color map origin
		short cmLength; //color map length
		unsigned char cmSize; //color map size

		short xOrigin; //bottom left x coordinate origin
		short yOrigin; //bottom left y coordinate origin

		short width; //picture width (in pixels)
		short height; //picture height (in pixels)

		unsigned char pixelDepth; //bits per pixel: 8, 16, 24 or 32
		unsigned char imageDescriptor; //24 bits = 0x00; 32 bits = 0x80
	};
	#pragma pack(pop)

	class LoaderTGA : public Loader<Image>
	{
		public:
			LoaderTGA();
			~LoaderTGA() override = default;

			Image *loadFromFile(const std::string &, void *) override;

		private:
			void getImageInfo(const TgaHeader &);
			void readTGA8bits();
			void readTGA16bits();
			void readTGA24bits();
			void readTGA32bits();
			void readTGAgray8bits();
			void readTGAgray16bits();
			void readTGA8bitsRLE();
			void readTGA16bitsRLE();
			void readTGA24bitsRLE();
			void readTGA32bitsRLE();
			void readTGAgray8bitsRLE();
			void readTGAgray16bitsRLE();

			//temporary attributes
			unsigned char *colorMap, *data;
		
			//attributes for Image
			unsigned int width, height;
			unsigned int componentsCount;
			Image::ImageFormat format;
			unsigned char *texels;
	};

}

#endif
