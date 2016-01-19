#ifndef ENGINE_TEXTUREFILTERBUILDER_H
#define ENGINE_TEXTUREFILTERBUILDER_H

#include <GL/gl.h>
#include <string>
#include <memory>
#include <stdexcept>

#include "TextureFilterBuilder.h"
#include "TextureFilter.h"
#include "utils/filter/downsample/DownSampleFilter.h"
#include "utils/filter/gaussianblur/GaussianBlurFilter.h"
#include "utils/filter/bilateralblur/BilateralBlurFilter.h"

namespace urchin
{

	template<typename T> class TextureFilterBuilder
	{
		public:
			TextureFilterBuilder();
			virtual ~TextureFilterBuilder();

			T *textureSize(unsigned int, unsigned int);
			unsigned int getTextureWidth() const;
			unsigned int getTextureHeight() const;

			T *textureType(unsigned int);
			unsigned int getTextureType() const;

			T *textureAccessFilter(unsigned int);
			unsigned int getTextureAccessFilter() const;

			T *textureAnisotropy(float);
			float getTextureAnisotropy() const;

			T *textureNumberLayer(unsigned int);
			unsigned int getTextureNumberLayer() const;

			T *textureInternalFormat(int);
			int getTextureInternalFormat() const;

			T *textureFormat(unsigned int);
			unsigned int getTextureFormat() const;

			virtual std::shared_ptr<TextureFilter> build() = 0;

		private:
			T* _this();

			unsigned int textureWidth, textureHeight;

			unsigned int pTextureType;
			unsigned int pTextureAccessFilter;
			float pTextureAnisotropy;
			unsigned int pTextureNumberLayer;
			int pTextureInternalFormat;
			unsigned int pTextureFormat;
	};

	#include "TextureFilterBuilder.inl"

}

#endif
