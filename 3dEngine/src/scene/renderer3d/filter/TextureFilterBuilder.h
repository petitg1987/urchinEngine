#ifndef ENGINE_TEXTUREFILTERBUILDER_H
#define ENGINE_TEXTUREFILTERBUILDER_H

#include <string>
#include <memory>

#include "TextureFilter.h"

namespace urchin
{

	class TextureFilterBuilder
	{
		public:
			TextureFilterBuilder();
			~TextureFilterBuilder();

			TextureFilterBuilder *filterType(TextureFilter::FilterType);
			TextureFilter::FilterType getFilterType() const;

			TextureFilterBuilder *textureSize(unsigned int, unsigned int);
			unsigned int getTextureWidth() const;
			unsigned int getTextureHeight() const;

			TextureFilterBuilder *textureType(unsigned int);
			unsigned int getTextureType() const;

			TextureFilterBuilder *textureNumberLayer(unsigned int);
			unsigned int getTextureNumberLayer() const;

			TextureFilterBuilder *textureInternalFormat(int);
			int getTextureInternalFormat() const;

			TextureFilterBuilder *textureFormat(unsigned int);
			unsigned int getTextureFormat() const;

			std::shared_ptr<TextureFilter> build();

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
