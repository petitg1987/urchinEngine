#ifndef ENGINE_BILATERALBLURFILTER_H
#define ENGINE_BILATERALBLURFILTER_H

#include <string>
#include <map>

#include "utils/filter/TextureFilter.h"

namespace urchin
{

	class BilateralBlurFilterBuilder;

	class BilateralBlurFilter : public TextureFilter
	{
		public:
			enum BlurDirection
			{
				VERTICAL,
				HORIZONTAL
			};

			BilateralBlurFilter(const BilateralBlurFilterBuilder *, BlurDirection);
			~BilateralBlurFilter();

			void onCameraProjectionUpdate(float, float);

		private:
			std::string getShaderName() const;
			void initializeAdditionalUniforms(unsigned int);
			void bindAdditionalTextures() const;
			void completeShaderTokens(std::map<std::string, std::string> &) const;

			std::vector<float> computeOffsets() const;

			BlurDirection blurDirection;
			unsigned int blurSize;
			float blurSharpness;
			unsigned int depthTextureID;

			unsigned int textureSize;
			int cameraPlanesLoc;

			std::string offsetsTab;
	};

}

#endif
