#ifndef ENGINE_BLURFILTER_H
#define ENGINE_BLURFILTER_H

#include <string>

#include "TextureFilter.h"

namespace urchin
{

	class TextureFilterBuilder;

	class BlurFilter : public TextureFilter
	{
		public:
			BlurFilter(const TextureFilterBuilder *);
			~BlurFilter();

		private:
			std::string getShaderName() const;
	};

}

#endif
