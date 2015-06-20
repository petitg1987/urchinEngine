#ifndef ENGINE_CUSTOMUNIFORM_H
#define ENGINE_CUSTOMUNIFORM_H

#include "UrchinCommon.h"

namespace urchin
{

	/**
	* Allow to load custom uniform for model displayer
	*/
	class CustomUniform
	{
		public:
			CustomUniform();
			virtual ~CustomUniform();

			virtual void loadCustomUniforms() = 0;

	};

}

#endif
