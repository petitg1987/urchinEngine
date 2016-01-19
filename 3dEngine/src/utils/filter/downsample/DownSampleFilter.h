#ifndef ENGINE_DOWNSAMPLEFILTER_H
#define ENGINE_DOWNSAMPLEFILTER_H

#include <string>
#include <map>

#include "utils/filter/TextureFilter.h"

namespace urchin
{

	class DownSampleFilterBuilder;

	class DownSampleFilter : public TextureFilter
	{
		public:
			DownSampleFilter(const DownSampleFilterBuilder *);
			~DownSampleFilter();

		private:
			std::string getShaderName() const;
			void completeShaderTokens(std::map<std::string, std::string> &) const;
	};

}

#endif
