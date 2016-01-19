#ifndef ENGINE_DOWNSAMPLEFILTERBUILDER_H
#define ENGINE_DOWNSAMPLEFILTERBUILDER_H

#include "utils/filter/TextureFilterBuilder.h"

namespace urchin
{

	class DownSampleFilterBuilder : public TextureFilterBuilder<DownSampleFilterBuilder>
	{
		public:
			DownSampleFilterBuilder();
			virtual ~DownSampleFilterBuilder();

			std::shared_ptr<TextureFilter> build();
			std::shared_ptr<DownSampleFilter> buildDownSample();
	};

}

#endif
