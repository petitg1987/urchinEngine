#ifndef ENGINE_CUSTOMMODELUNIFORM_H
#define ENGINE_CUSTOMMODELUNIFORM_H

#include "UrchinCommon.h"

#include "scene/renderer3d/model/Model.h"

namespace urchin
{

	/**
	* Allow to load custom model uniform for model displayer
	*/
	class CustomModelUniform
	{
		public:
			CustomModelUniform();
			virtual ~CustomModelUniform();

			virtual void loadCustomUniforms(const Model *) = 0;

	};

}

#endif
