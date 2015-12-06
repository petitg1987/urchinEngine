#ifndef ENGINE_DOWNSAMPLEFILTER_H
#define ENGINE_DOWNSAMPLEFILTER_H

#include <string>

#include "TextureFilter.h"

namespace urchin
{

	class TextureFilterBuilder;

	class DownSampleFilter : public TextureFilter
	{
		public:
			DownSampleFilter(const TextureFilterBuilder *);
			~DownSampleFilter();

		private:
			std::string getShaderName() const;
			void completeShaderTokens(std::map<std::string, std::string> &) const;
	};

}

#endif
