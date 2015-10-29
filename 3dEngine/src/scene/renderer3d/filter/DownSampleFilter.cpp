#include "DownSampleFilter.h"
#include "TextureFilterBuilder.h"

namespace urchin
{

	DownSampleFilter::DownSampleFilter(const TextureFilterBuilder *textureFilterBuilder):
		TextureFilter(textureFilterBuilder)
	{

	}

	DownSampleFilter::~DownSampleFilter()
	{

	}

	std::string DownSampleFilter::getShaderName() const
	{
		return "downSampleTex";
	}

}
