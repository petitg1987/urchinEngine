#ifndef URCHINENGINE_GAUSSIANBLURFILTERBUILDER_H
#define URCHINENGINE_GAUSSIANBLURFILTERBUILDER_H

#include "utils/filter/TextureFilterBuilder.h"

namespace urchin
{

	class GaussianBlurFilterBuilder : public TextureFilterBuilder<GaussianBlurFilterBuilder>
	{
		public:
			enum BlurDirection
			{
				HORIZONTAL_BLUR,
				VERTICAL_BLUR
			};

			GaussianBlurFilterBuilder();
			virtual ~GaussianBlurFilterBuilder();

			GaussianBlurFilterBuilder *blurDirection(BlurDirection);

			GaussianBlurFilterBuilder *blurSize(unsigned int);
			unsigned int getBlurSize() const;

			std::shared_ptr<TextureFilter> build();
			std::shared_ptr<GaussianBlurFilter> buildGaussianBlur();

		private:
			BlurDirection pBlurDirection;
			unsigned int pBlurSize;
	};

}

#endif
