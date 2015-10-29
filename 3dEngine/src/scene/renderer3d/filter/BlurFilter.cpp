#include "BlurFilter.h"
#include "TextureFilterBuilder.h"

namespace urchin
{

	BlurFilter::BlurFilter(const TextureFilterBuilder *textureFilterBuilder):
		TextureFilter(textureFilterBuilder)
	{

	}

	BlurFilter::~BlurFilter()
	{

	}

	std::string BlurFilter::getShaderName() const
	{
		return "blurTex";
	}

}
