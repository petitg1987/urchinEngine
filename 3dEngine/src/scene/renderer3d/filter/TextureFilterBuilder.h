#ifndef ENGINE_TEXTUREFILTERBUILDER_H
#define ENGINE_TEXTUREFILTERBUILDER_H

#include <string>
#include <memory>

#include "scene/renderer3d/filter/TextureFilter.h"

namespace urchin
{

	class TextureFilterBuilder
	{
		public:
			enum FilterType
			{
				DOWN_SAMPLE,
				GAUSSIAN_BLUR_V,
				GAUSSIAN_BLUR_H
			};

			TextureFilterBuilder();
			~TextureFilterBuilder();

			TextureFilterBuilder *filterType(FilterType);

			TextureFilterBuilder *textureSize(unsigned int, unsigned int);
			unsigned int getTextureWidth() const;
			unsigned int getTextureHeight() const;

			TextureFilterBuilder *textureType(unsigned int);
			unsigned int getTextureType() const;

			TextureFilterBuilder *textureAccessFilter(unsigned int);
			unsigned int getTextureAccessFilter() const;

			TextureFilterBuilder *textureAnisotropy(float);
			float getTextureAnisotropy() const;

			TextureFilterBuilder *textureNumberLayer(unsigned int);
			unsigned int getTextureNumberLayer() const;

			TextureFilterBuilder *textureInternalFormat(int);
			int getTextureInternalFormat() const;

			TextureFilterBuilder *textureFormat(unsigned int);
			unsigned int getTextureFormat() const;

			//Blur specific
			TextureFilterBuilder *blurSize(unsigned int);
			unsigned int getBlurSize() const;

			std::shared_ptr<TextureFilter> build();

		private:
			FilterType pFilterType;
			unsigned int textureWidth, textureHeight;

			unsigned int pTextureType;
			unsigned int pTextureAccessFilter;
			float pTextureAnisotropy;
			unsigned int pTextureNumberLayer;
			int pTextureInternalFormat;
			unsigned int pTextureFormat;

			//Blur specific
			unsigned int pBlurSize;
	};

}

#endif
