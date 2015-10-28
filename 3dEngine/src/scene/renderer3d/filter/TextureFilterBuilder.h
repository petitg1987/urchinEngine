#ifndef ENGINE_TEXTUREFILTERBUILDER_H
#define ENGINE_TEXTUREFILTERBUILDER_H

#include <string>

#include "TextureFilter.h"

namespace urchin
{

	class TextureFilterBuilder
	{
		public:
			TextureFilterBuilder();
			~TextureFilterBuilder();

			TextureFilterBuilder *filterType(TextureFilter::FilterType);
			TextureFilterBuilder *textureSize(unsigned int, unsigned int);
			TextureFilterBuilder *textureType(unsigned int);
			TextureFilterBuilder *textureNumberLayer(unsigned int);
			TextureFilterBuilder *textureInternalFormat(int);
			TextureFilterBuilder *textureFormat(unsigned int);

			TextureFilter *build();

		private:
			TextureFilter::FilterType pFilterType;
			unsigned int textureWidth, textureHeight;

			unsigned int pTextureType;
			unsigned int pTextureNumberLayer;
			int pTextureInternalFormat;
			unsigned int pTextureFormat;
	};

}

#endif
