#ifndef ENGINE_SHADOWMODELUNIFORM_H
#define ENGINE_SHADOWMODELUNIFORM_H

#include "UrchinCommon.h"

#include "scene/renderer3d/shadow/data/ShadowData.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/displayer/CustomModelUniform.h"

namespace urchin
{

	/**
	* Load custom model uniform for shadow model displayer
	*/
	class ShadowModelUniform : public CustomModelUniform
	{
		public:
			ShadowModelUniform();
			~ShadowModelUniform();

			void setSplitsToUpdateLocation(int);
			void setModelUniformData(const ShadowData *);

			void loadCustomUniforms(const Model *);

		private:
			int splitsToUpdateLoc;
			const ShadowData *shadowData;
	};

}

#endif
