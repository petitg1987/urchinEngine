#ifndef URCHINENGINE_DOWNSAMPLEFILTERBUILDER_H
#define URCHINENGINE_DOWNSAMPLEFILTERBUILDER_H

#include "utils/filter/TextureFilterBuilder.h"

namespace urchin
{

	class DownSampleFilterBuilder : public TextureFilterBuilder<DownSampleFilterBuilder>
	{
		public:
			DownSampleFilterBuilder();
			virtual ~DownSampleFilterBuilder() = default;

			std::shared_ptr<TextureFilter> build();
			std::shared_ptr<DownSampleFilter> buildDownSample();
	};

}

#endif
