#ifndef URCHINENGINE_CUSTOMMODELUNIFORM_H
#define URCHINENGINE_CUSTOMMODELUNIFORM_H

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
			virtual ~CustomModelUniform() = default;

			virtual void loadCustomUniforms(const Model *) = 0;

	};

}

#endif
