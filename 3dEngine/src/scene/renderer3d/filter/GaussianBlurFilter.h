#ifndef ENGINE_BLURFILTER_H
#define ENGINE_BLURFILTER_H

#include <string>
#include <vector>

#include "TextureFilter.h"

namespace urchin
{

	class TextureFilterBuilder;

	class GaussianBlurFilter : public TextureFilter
	{
		public:
			enum GaussianDirection
			{
				VERTICAL,
				HORIZONTAL
			};

			GaussianBlurFilter(const TextureFilterBuilder *, GaussianDirection);
			~GaussianBlurFilter();

		private:
			std::string getShaderName() const;
			void completeShaderTokens(std::map<std::string, std::string> &) const;

			std::vector<float> computeWeights() const;
			std::vector<float> computeWeightsLinearSampling(const std::vector<float> &) const;
			std::vector<float> computeOffsetsLinearSampling(const std::vector<float> &, const std::vector<float> &) const;
			std::string toShaderVectorValues(std::vector<float> &) const;

			GaussianDirection gaussianDirection;
			unsigned int blurSize;

			unsigned int nbTextureFetch;
			unsigned int textureSize;
			std::string weightsTab;
			std::string offsetsTab;
	};

}

#endif
